#!/usr/bin/env python2

import subprocess
from subprocess import Popen, PIPE
from threading import Timer
import os, glob
from math import *
import random
import sys



sourcename="goldbach_plus"
sourcefiles=[sourcename+".c"]
helper_sourcefiles=['goldbach_plus_test_helpers.c']
flags=['--std=c99','-g','-lm']
wrapped_functions=['MPI_Init','MPI_Finalize','MPI_Send'];
exename='./'+sourcename
default_timeout=30

RED='\033[91m'
GREEN='\033[92m'
ENDC='\033[0m'

def fail():
    print "This code is "+RED+" not acceptable"+ENDC
    exit(1)

def succeed():
    print "This code is "+GREEN+"acceptable"+ENDC
    exit(0)

def ref_dir():
    d="./"
    for a in sys.argv[1:]:
        if a[:12]!='--max-procs=':
            d=sys.argv[1]+'/'
    return d;

def run_command(command="",abort_on_failure=True,abort_on_timeout=True,timeout_returncode=-2,print_command=True,print_output=False,timeout=default_timeout):
    if print_command:
        print "Executing command:"
        print " ".join(command)
    P=Popen(command,stdin=PIPE,stdout=PIPE,stderr=PIPE)
    timer=Timer(timeout,lambda p:p.kill(),[P])
    timer.daemon=True;
    timer.start()
    P.wait()
    [s1,s2]=P.communicate();
    if timer.isAlive():
        timer.cancel()
    else:
        if abort_on_timeout:
            print "It looks like this code entered an infinite loop, or was stuck waiting for input, or just refused to exit.  I had to kill it manually. "
            fail()
        else:
            returncode=timeout_returncode
    
    if P.returncode != 0 and abort_on_failure:
        print s1
        print s2
        print "Error.  The return code was non-zero, meaning that this code crashed."
        fail()
    returncode=P.returncode

    if print_output:
        print s1
        print s2
    return returncode,s1,s2

def run_instance(standard,np=3,p=10,print_output=True,chunksize=None,timeout=default_timeout):
    if not standard:
        for f in glob.glob('whatidid.*'):
            os.remove(f)
    default_max=75;
    if standard==False:
        command=['mpirun','-np',str(np),'--oversubscribe',exename]
    else:
        command=['mpirun','-np',str(np),'--oversubscribe',ref_dir()+exename+'_standard']

    command+=['--p='+str(p)]

    if chunksize:
        command=command+['--chunksize='+str(chunksize)]
    returncode,s1,s2=run_command(command,print_output=print_output,timeout=timeout)
    return returncode,s1,s2

def check_output_correctness(s1,standard_s1):
    def reduce(x):
        y=[s for s in x.split("\n") if 'executing' not in s.lower() and 'mpirun' not in s.lower() and 'finding' not in s.lower()][0]
        return y

        
    partitions=reduce(s1);
    partitions_standard=reduce(standard_s1)

    if partitions!=partitions_standard:
        print "Error, your results are incorrect"
        fail()

        


def main(argv):
    if len(argv)>1 and argv[1][:12]=='--max-procs=':
        max_procs=int(argv[1][12:])
    else:
        max_procs=180
            
    mpicccommand=["mpicc"]+flags+map(lambda x:'-Wl,-wrap,'+x,wrapped_functions)+sourcefiles+map(lambda x:ref_dir()+x,helper_sourcefiles)+["-o",exename]
    gcc_status=run_command(mpicccommand,timeout=2,print_output=True)[0];
    if gcc_status != 0:
        print "Error, gcc either had warnings or the code completely failed to compile.  Fix these things before proceeding.  The compiler is your friend, and is trying to help you."
        fail()
    
    width=100
    
    #Test correctness of output, in several cases
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,p=3);
    dummy,s1_stand,s2_stand=run_instance(standard=True,p=3)
    check_output_correctness(s1,s1_stand)

    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,p=3,np=10);
    dummy,s1_stand,s2_stand=run_instance(standard=True,p=3,np=10)
    check_output_correctness(s1,s1_stand)

    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,p=10);
    dummy,s1_stand,s2_stand=run_instance(standard=True,p=10)
    check_output_correctness(s1,s1_stand)

    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,p=5,np=10);
    dummy,s1_stand,s2_stand=run_instance(standard=True,p=5,np=10)
    check_output_correctness(s1,s1_stand)
    
    #Test on a big one. 
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,p=50,np=7);
    dummy,s1_stand,s2_stand=run_instance(standard=True,p=50,np=7);
    check_output_correctness(s1,s1_stand)

    #Make sure that the root is not doing any work:
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,p=50,np=3);
    L=open('whatidid.rank.0','r').readlines();
    root_proc_calls=int(L[2].split(':')[1])
    if root_proc_calls!=0:
        print "Error, the root should not be doing any work, but it called goldbach_partition "+str(root_proc_calls)+" times"
        fail()


    #make sure that we are only sending ranges to check, not lists to check. 
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,p=100,np=3);
    L=open('whatidid.rank.0','r').readlines();
    root_proc_sends=int(L[1].split(':')[1])
    root_proc_send_data_count=int(L[3].split(':')[1])
    if root_proc_send_data_count>4*root_proc_sends:
        print "Error, the root performed "+str(root_proc_sends)+" sends, but those sends contained "+str(root_proc_send_data_count)+" data.  That is wayyy, too much.  You only need to send a range (top,bottom) to describe a work task.  Not a whole list"
        fail()

    
    # #Finally, test the efficiency
    P=500
    np=[2,3,5,9,17,33];

    np=[x for x in np if x<=max_procs];
    max_runtime=[]

    if len(np)>0:
        if max(np)<33:
            print RED+"Warning: You have turned down the maximum number of processors to run efficiency tests with.  This is fine for testing, but I will run on grendel with up to 33 processes.  You should check your code there before considering the code done"+ENDC
        
        for p in np:
            run_instance(standard=False,np=p,timeout=180,p=P)
            max_runtime.append(0);
            for p1 in range(p):
                L=open('whatidid.rank.'+str(p1)).readlines();
                max_runtime[-1]=max([max_runtime[-1],float(L[0].split(':')[1])])

        eff=[1.0*max_runtime[0]/(max_runtime[i]*(np[i]-1)) for i in range(len(np))]
        eff=eff[1:]
        
        if sum(eff)/len(eff) < 0.5:
            print "Error.  Your code should have an average efficiency of at least 0.6.  Here are your runtimes with "+str(np)+"processes:"+str([round(x,2) for x in max_runtime])+"."
            print "Your efficiencies are therefore (counting only workers in the calculation, not the master): "+str([round(x,2) for x in eff])
            print "Your average efficiency is: "+str(round(sum(eff)/len(eff),2))
            print "Probably you are not implementing the dynamic load balancing very well"
            fail()
    else:
        print RED+"Skipping efficiency test"+ENDC


    
    succeed() 


if __name__=='__main__':
    main(sys.argv)
