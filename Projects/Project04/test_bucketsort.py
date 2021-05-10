#!/usr/bin/env python2

import subprocess
from subprocess import Popen, PIPE
from threading import Timer
import os, glob
from math import *
import random
import sys
import time


sourcename="bucketsort"
sourcefiles=[sourcename+".c"]
helper_sourcefiles=['bucketsort_test_helpers.c']
flags=['--std=c99','-g','-lm','-Wall']
wrapped_functions=['MPI_Init','MPI_Finalize','MPI_Send','MPI_Allgatherv','MPI_Alltoall','MPI_Scatter','MPI_Gather','MPI_Allgather','MPI_Alltoallv','MPI_Scatterv','MPI_Gatherv','MPI_Isend'];
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

def run_instance(standard,np=3,print_results=None,inputfile=None,make_outputfile=True,P=50,timeout=default_timeout,print_output=True):
    if not standard:
        for f in glob.glob('whatidid.*'):
            os.remove(f)
    default_max=75;
    if standard==False:
        command=['mpirun','-np',str(np),'--oversubscribe',exename]
    else:
        command=['mpirun','-np',str(np),'--oversubscribe',ref_dir()+exename+'_standard']
    if print_results is not None:
        command=command+['--print='+str(print_results)];
    if inputfile is not None:
        command=command+['--input_file='+inputfile];
    else:
        command=command+['--n='+str(P)]
    if standard:
        command=command+['--output_file=sorted_standard.txt']
    else:
        command=command+['--output_file=sorted.txt']
    if make_outputfile:
        command=command+['--produce_outputfile=1']
    else:
        command=command+['--produce_outputfile=0']

    returncode,s1,s2=run_command(command,print_output=print_output,timeout=timeout)
    return returncode,s1,s2

def check_output_correctness(s1,standard_s1):
    retrieve_list_from_output=lambda s: [l.split(':')[1].strip() for l in s.split('\n') if len(l)>0 and l[0]=='L' and ':' in l]
    s1_numbers=retrieve_list_from_output(s1)
    s1_standard_numbers=retrieve_list_from_output(standard_s1)

    if s1_numbers != s1_standard_numbers:
        print "Error, at least one of your results that got printed to the terminal is incorrect.  Try running your code and the standard code using the commands listed above, and see what the difference is. Note that the order in which you list the results does not matter."
        fail()

    s1_numbers=open('sorted.txt','r').readlines();
    s1_standard_numbers=open('sorted_standard.txt','r').readlines();
    if s1_numbers != s1_standard_numbers:
        print "Error, at load one of your results that got printed to the output file is incorrect. Compare sorted.txt and sorted_standard.txt to see what the problem is."
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
    dummy,s1,s2=run_instance(standard=False);
    dummy,s1_stand,s2_stand=run_instance(standard=True)
    check_output_correctness(s1,s1_stand)

    #Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,P=1000);
    dummy,s1_stand,s2_stand=run_instance(standard=True,P=1000)
    check_output_correctness(s1,s1_stand)

    #Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,P=75,print_results=1);
    dummy,s1_stand,s2_stand=run_instance(standard=True,P=75,print_results=1)
    check_output_correctness(s1,s1_stand)

    #Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,P=25,print_results=0);
    dummy,s1_stand,s2_stand=run_instance(standard=True,P=25,print_results=0)
    check_output_correctness(s1,s1_stand)


    #Test a couple random cases, test whether or not hte code respects the --print parameter.
    print "".join(['=' for i in range(width)]);
    dummy,s1,s2=run_instance(standard=False,inputfile="unsorted_list.txt",timeout=60);
    dummy,s1_stand,s2_stand=run_instance(standard=True,inputfile="unsorted_list.txt",timeout=60)
    check_output_correctness(s1,s1_stand)



    #Finally, test the performance, along with number of messages sent.
    N=int(1e8)
    np=[1,2,4,8,16,32];

    np=[x for x in np if x<=max_procs];
    max_runtime=[]

    if len(np)>0:
        if max(np)<32:
            print RED+"Warning: You have turned down the maximum number of processors to run efficiency tests with.  This is fine for testing, but I will run on grendel with up to 32 processes.  You should check your code there before considering the code done"+ENDC
            print "Similarly, I am turning down the size of the data that you are running on, to help it get done faster.  You need to take these results lightly."
            N=1e6

        for p in np:
            print "".join(['=' for i in range(width)]);
            s=time.time()
            dummy,s1,s2=run_instance(standard=False,P=N,print_results=0,np=p,timeout=120,make_outputfile=False);
            e=time.time();
            et=e-s;
            s=time.time();
            dummy,s1_stand,s2_stand=run_instance(standard=True,P=N,print_results=0,np=p,timeout=120,make_outputfile=False)
            e=time.time()
            standard_et=e-s;
            if (et>2.5*standard_et):
                print "Error: This code took "+str(et)+" seconds to run, when the standard took "+str(standard_et)
                fail()

            #calculate the miscellaneous send total
            misc_sends=sum([int(open('whatidid.rank.'+str(x)+'.txt').readlines()[1].split(':')[1].strip()) for x in range(p)])
            if misc_sends > 10 * p:
                print "Error, the code did a total of "+str(misc_sends)+" miscellaneous sends, which is more than "+str(10*p)+".  That is wayy more than needed."
                fail()
            #only rank 0 should do a send via scatterv and it should be N long.
            scatterv_sends=[int(open('whatidid.rank.'+str(x)+'.txt').readlines()[4].split(':')[1].strip()) for x in range(p)]
            if any([s > 0 for s in scatterv_sends[1:]]):
                print "Error! Only rank 0 should be the root on a scatterv in this algorithm!"
                fail()
            if sum(scatterv_sends)!=N:
                print "Error! rank 0 should scatterv out exactly the initial list, and nothing else. "
                fail()

            alltoallv_sends=[int(open('whatidid.rank.'+str(x)+'.txt').readlines()[2].split(':')[1].strip()) for x in range(p)]
            gatherv_sends=[int(open('whatidid.rank.'+str(x)+'.txt').readlines()[5].split(':')[1].strip()) for x in range(p)]

            allowable_range=[0.7*N/p,1.3*N/p]
            if p>1 and any([allowable_range[0]>x or x>allowable_range[1] for x in alltoallv_sends]):
                print "Error!  Each processor should send between "+str(allowable_range[0])+" and "+str(allowable_range[1])+" data via alltoallv.  Here is a list of how many sends each processor actually did. "+str(alltoallv_sends)
                fail()

            if p>1 and any([allowable_range[0]>x or x>allowable_range[1] for x in gatherv_sends]):
                print "Error!  Each processor should send between "+str(allowable_range[0])+" and "+str(allowable_range[1])+" data via gatherv.  Here is a list of how many sends each processor actually did. "+str(gatherv_sends)
                fail()

    else:
        print RED+"Skipping performance test"+ENDC

    succeed()


if __name__=='__main__':
    main(sys.argv)
