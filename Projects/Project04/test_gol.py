#!/usr/bin/env python2

import subprocess
from subprocess import Popen, PIPE
from threading import Timer
import os, glob
from math import *
import random
import sys
import time


sourcename="gol"
sourcefiles=[sourcename+".c"]
helper_sourcefiles=['gol_test_helpers.c']
flags=['--std=c99','-g','-lm','-Wall']
wrapped_functions=['MPI_Init','MPI_Finalize','MPI_Send','MPI_Allgatherv','MPI_Alltoall','MPI_Scatter','MPI_Gather','MPI_Allgather','MPI_Alltoallv','MPI_Scatterv','MPI_Gatherv','MPI_Isend','MPI_Sendrecv'];
exename='./'+sourcename
default_timeout=30



filesdir='gol_files/'

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

def run_instance(standard,np=3,inputfile=None,outputfile=True,generations=100,timeout=default_timeout,print_output=True):
    if not standard:
        for f in glob.glob('whatidid.*'):
            os.remove(f)
    if standard==False:
        command=['mpirun','-np',str(np),'--oversubscribe',exename]
    else:
        command=['mpirun','-np',str(np),'--oversubscribe', ref_dir()+exename+'_standard']
    if inputfile is not None:
        command=command+['--inputfile='+inputfile];
    if outputfile is not None:
        command=command+['--outputfile='+outputfile];
    command=command+['--generations='+str(generations)]

    returncode,s1,s2=run_command(command,print_output=print_output,timeout=timeout)
    return returncode,s1,s2

def check_output_correctness(filename1,filename2):
    s=run_command(['diff',filename1,filename2],abort_on_failure=False)[0]
    if s!=0:
        print "Error, the produced files "+filename1+" and "+filename2+" are not identical; they should be.  Sorry that I don't have better information for you."

        fail()

def main(argv):
    if len(argv)>1:
        if argv[1][:12]=='--max-procs=':
            max_procs=int(argv[1][12:])
        else:
            print "Error, option "+argv[1]+" not recognized."
            exit()
    else:
         max_procs=180

    mpicccommand=["mpicc"]+flags+map(lambda x:'-Wl,-wrap,'+x,wrapped_functions)+sourcefiles+map(lambda x:ref_dir()+x,helper_sourcefiles)+["-o",exename]
    gcc_status=run_command(mpicccommand,timeout=2)[0];
    if gcc_status != 0:
        print "Error, gcc either had warnings or the code completely failed to compile.  Fix these things before proceeding.  The compiler is your friend, and is trying to help you."
        fail()

    width=100
    # Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    case='pulsar'
    dummy,s1,s2=run_instance(standard=False,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'.gif');
    dummy,s1_stand,s2_stand=run_instance(standard=True,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'_standard.gif')
    check_output_correctness(filesdir+case+'.gif',filesdir+case+'_standard.gif')


   # Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    case='blinker'
    dummy,s1,s2=run_instance(np=1,standard=False,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'.gif');
    dummy,s1_stand,s2_stand=run_instance(np=1,standard=True,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'_standard.gif')
    check_output_correctness(filesdir+case+'.gif',filesdir+case+'_standard.gif')

    # Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    case='beehive'
    dummy,s1,s2=run_instance(np=4,standard=False,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'.gif',generations=500);
    dummy,s1_stand,s2_stand=run_instance(np=4,standard=True,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'_standard.gif',generations=500)
    check_output_correctness(filesdir+case+'.gif',filesdir+case+'_standard.gif')   

    # Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    case='glider'
    dummy,s1,s2=run_instance(np=8,standard=False,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'.gif',generations=500);
    dummy,s1_stand,s2_stand=run_instance(np=8,standard=True,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'_standard.gif',generations=500)
    check_output_correctness(filesdir+case+'.gif',filesdir+case+'_standard.gif')   

    
    # Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    case='rand_25x113'
    dummy,s1,s2=run_instance(standard=False,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'.gif',generations=50);
    dummy,s1_stand,s2_stand=run_instance(standard=True,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'_standard.gif',generations=50)
    check_output_correctness(filesdir+case+'.gif',filesdir+case+'_standard.gif')  
    
 
    #Finally, test the performance, along with number of messages sent.
    case='rand_100x100'
    np=[1,2,4,8,16,32];

    np=[x for x in np if x<=max_procs];

    if len(np)>0:
        if max(np)<32:
            print RED+"Warning: You have turned down the maximum number of processors to run efficiency tests with.  This is fine for testing, but I will run on grendel with up to 32 processes.  You should check your code there before considering the code done"+ENDC
            print "Similarly, I am turning down the size of the data that you are running on, to help it get done faster.  You need to take these results lightly."
            N=1e6

        for p in np:

            print "".join(['=' for i in range(width)]);
            s=time.time()
            dummy,s1,s2=run_instance(np=p,standard=False,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'.gif',generations=50);
            e=time.time();
            et=e-s;
            s=time.time();
            dummy,s1_stand,s2_stand=run_instance(np=p,standard=True,inputfile=filesdir+case+'.txt',outputfile=filesdir+case+'_standard.gif',generations=50)
            e=time.time()
            standard_et=e-s;
            if (et>2.5*standard_et):
                print "Error: This code took "+str(et)+" seconds to run, when the standard took "+str(standard_et)
                fail()

            #calculate the set_state calls
            set_states=[int(open('whatidid.rank.'+str(x)+'.txt').readlines()[3].split(':')[1].strip()) for x in range(p)]
            #They should all be within 20% of the mean
            mean_set_states=1.0*sum(set_states)/len(set_states);
            allowable_range=[0.8*mean_set_states,1.2*mean_set_states]
            if any([s<allowable_range[0] or s>allowable_range[1] for s in set_states]):
                print "Error, each processor should call set_states beetween "+str(allowable_range[0]) + " and "+str(allowable_range[1])+" times.  Here are the number of times your processors called set_states"+str(set_states)
                fail()

            #calculate the miscellaneous send total, should be less than 2.2*set state calls
            misc_sends=[int(open('whatidid.rank.'+str(x)+'.txt').readlines()[1].split(':')[1].strip()) for x in range(p)]
            for pi in range(p):
                if misc_sends[pi]>2.5*set_states[pi]:
                    print "Error, on rank "+str(pi)+ " the code did a total of "+str(misc_sends[pi])+" miscellaneous sends, which is more than "+str(2.5*set_states[pi])+".  That is wayy more than needed."
                    fail()
                
            #calculate the number of MPI_sendrecv calls, there should be at most 30% the number of set_state calls of them
            sendrecv_sends=[int(open('whatidid.rank.'+str(x)+'.txt').readlines()[2].split(':')[1].strip()) for x in range(p)]
            for pi in range(p):
                if sendrecv_sends[pi]>0.3*set_states[pi]:
                    print "Error! rank "+str(pi)+" did "+str(sendrecv_sends[pi])+" sends via sendrecv.  This is way too much.  Each processor should only sendrecv the *border* of its block"
                    fail()
 
    else:
        print RED+"Skipping performance test"+ENDC

    succeed()


if __name__=='__main__':
    main(sys.argv)
