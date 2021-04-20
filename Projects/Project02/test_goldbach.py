#!/usr/bin/env python2


import subprocess
from subprocess import Popen, PIPE
from threading import Timer
import os, glob
from math import *
import random
import sys



sourcename="goldbach"
sourcefiles=[sourcename+".c"]
helper_sourcefiles=['goldbach_test_helpers.c']
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

def run_instance(standard,np=3,print_results=None,domax=[],dolist=[],dorange=[],print_output=True,chunksize=None,timeout=default_timeout):
    if not standard:
        for f in glob.glob('whatidid.*'):
            os.remove(f)
    default_max=75;
    if standard==False:
        command=['mpirun','-np',str(np),'--oversubscribe',exename]
    else:
        command=['mpirun','-np',str(np),'--oversubscribe',ref_dir()+exename+'_standard']
    if domax:
        command=command+['--max='+str(domax)];
    elif dolist:
        command=command+['--list']+map(str,dolist);
    elif dorange:
        command=command+['--range='+str(dorange[0])+'/'+str(dorange[1])]
    else:
        command=command+['--max='+str(default_max)]
    if print_results is not None:
        D={True:'1',False:'0'}
        command=command+['--print_results='+D[print_results]]
    if chunksize:
        command=command+['--chunksize='+str(chunksize)]
    returncode,s1,s2=run_command(command,print_output=print_output,timeout=timeout)
    return returncode,s1,s2

def check_output_correctness(s1,standard_s1):
    partitions=sorted([map(int,s.replace('/',':').split(':')) for s in s1.split('\n') if s.split(":")[0].isdigit()],key=lambda x: x[0]);
    partitions_standard=sorted([map(int,s.replace('/',':').split(':')) for s in standard_s1.split('\n') if s.split(":")[0].isdigit()],key=lambda x: x[0]);

    if partitions!=partitions_standard:
        print "Error, at least one of your results is incorrect.  Try running your code and the standard code using the commands listed above, and see what the difference is. Note that the order in which you list the results does not matter."
        fail()

        


def main(argv):
    print(str(len(argv))+' arguments are being passed in')
    if len(argv)>1 and argv[1][:12]=='--max-procs=':
        max_procs=int(argv[1][12:])
    else:
        max_procs=180
   
    mpicccommand=["mpicc"]+flags+map(lambda x:'-Wl,-wrap,'+x,wrapped_functions)+sourcefiles+map(lambda x:ref_dir()+x,helper_sourcefiles)+["-o",exename]
    gcc_status=run_command(mpicccommand,timeout=2)[0];
    if gcc_status != 0:
        print "Error, gcc either had warnings or the code completely failed to compile.  Fix these things before proceeding.  The compiler is your friend, and is trying to help you."
        fail()
    
    width=100
    #Test correctness of output, and whether or not the code respect the --print_results parameter
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False);
    dummy,s1_stand,s2_stand=run_instance(standard=True)
    check_output_correctness(s1,s1_stand)

    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,print_results=False);
    dummy,s1_stand,s2_stand=run_instance(standard=True,print_results=False);
    check_output_correctness(s1,s1_stand)


    #Test to see if the list option is supported properly
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,dolist=[8,43,23,12,12,232,1291,2323,12,12013921,23,12913,239,4]);
    dummy,s1_stand,s2_stand=run_instance(standard=True,dolist=[8,43,23,12,12,232,1291,2323,12,12013921,23,12913,239,4]);
    check_output_correctness(s1,s1_stand)


    #Test on a big range.  This is the real test that everything is correct
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,dorange=[100,100000]);
    dummy,s1_stand,s2_stand=run_instance(standard=True,dorange=[10,100000]);
    check_output_correctness(s1,s1_stand)
    

    #Same, but fewer data than processors, just to see what happens. 
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,dolist=[8,43,23],np=5);
    dummy,s1_stand,s2_stand=run_instance(standard=True,dolist=[8,43,23],np=5);
    check_output_correctness(s1,s1_stand)


    #ok, now check to see if chunksize is supported --also, check to make sure that rank 0 is not doing anything.
    #also, check to make sure that partition_goldbach is being called exactly the right number of times overall. 
    datalen=1e4;
    np=4;
    chunksize=[10,20,50]
    total_number_sends=[]
    for c in chunksize:
        print "".join(['=' for i in range(width)]);
        dummy,s1,s2=run_instance(standard=False,domax=datalen,np=np,chunksize=c);
        total_number_sends.append(0);
        total_number_partition_calls=0;
        for p in range(np):
            L=open('whatidid.rank.'+str(p)).readlines()
            total_number_sends[-1]+=int(L[1].split(':')[1])
            total_number_partition_calls+=int(L[2].split(':')[1])
            if p==0 and int(L[2].split(':')[1])!=0:
                print "Error.  Rank 0 should not be doing any work, but it called goldback_partition "+L[2].split(':')[1]+' times'
                fail()
        if total_number_partition_calls<0.95*datalen or total_number_partition_calls>1.05*datalen:
            print "Error. There were "+str(datalen)+" numbers to partition, but overall, across all processes, goldbach_partition was called "+str(total_number_partition_calls)+" times.  That's not right!"
            fail()
    for i in range(1,len(chunksize)):
        acceptable_range=[0.9*chunksize[0]/chunksize[i]*total_number_sends[0],1.1*chunksize[0]/chunksize[i]*total_number_sends[0]];
        if  acceptable_range[0]>=total_number_sends[i] or acceptable_range[1]<=total_number_sends[i]:
            print "Error, with a chunksize of "+str(chunksize[0])+" the code did "+str(total_number_sends[0])+" sends.  With a chunksize of "+str(chunksize[i])+" the code should do between "+str(acceptable_range[0])+" and "+str(acceptable_range[1])+" sends, however, it did "+str(total_number_sends[i])+". You are probably not implementing the chunksize option correctly."
            fail()


    #Finally, test the efficiency
    datarange=[10,1e7]
    np=[2,3,5,9,17,33];

    np=[x for x in np if x<=max_procs];
    max_runtime=[]

    if len(np)>0:
        if max(np)<33:
            print RED+"Warning: You have turned down the maximum number of processors to run efficiency tests with.  This is fine for testing, but I will run on grendel with up to 33 processes.  You should check your code there before considering the code done"+ENDC
            print "Similarly, I am turning down the size of the data that you are running on, to help it get done faster.  You need to take these results lightly."
            datarange=[10,1e5]
        
        for p in np:
            run_instance(standard=False,dorange=datarange,np=p,timeout=300,chunksize=50000)
            max_runtime.append(0);
            for p1 in range(p):
                L=open('whatidid.rank.'+str(p1)).readlines();
                max_runtime[-1]=max([max_runtime[-1],float(L[0].split(':')[1])])

        eff=[1.0*max_runtime[0]/(max_runtime[i]*(np[i]-1)) for i in range(len(np))]
        eff=eff[1:]
        T=max_runtime;
        print "Here are the runtimes: "+str(T)
        print "Here are the proc nums: "+str(np)
        eff1=[1.0*T[i]*(np[i]-1)/(T[j]*(np[j]-1)) for i in range(len(np)) for j in range(len(np)) if j > i]
#        print "Under measure 0 we get eff0="+str(sum(eff)/len(eff))+" under measure 1 we get "+str(eff1)
#        print "The final statistic under measure 1 is "+str(sum(eff1)/len(eff1))
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
