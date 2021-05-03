#!/usr/bin/env python2

import subprocess
from subprocess import Popen, PIPE
from threading import Timer
import os, glob
import sys

RED='\033[91m'
GREEN='\033[92m'
ENDC='\033[0m'

def fail():
    print "This code is "+RED+" not acceptable"+ENDC
    exit(1)

def succeed():
    print "This code is "+GREEN+"acceptable"+ENDC
    exit(0)

ref_dir='./'
if len(sys.argv)>1:
    ref_dir=sys.argv[1]+'/'

sourcename="my_scatter.c"
exename="my_scatter_exe"
timeout=20

mpicccommand=["mpicc","--std=c99",'-g','-lm','-Wl,-wrap,MPI_Recv',"-Wl,-wrap,MPI_Send", "-Wl,-wrap,main", sourcename, ref_dir+"my_scatter_test_driver.c",ref_dir+"mpi_send_wrapper.c",ref_dir+"test_helpers.c","-o",exename]
print "Compiling your "+sourcename+" file ...."
print " ".join(mpicccommand)
gcc_status=subprocess.call(mpicccommand)
if gcc_status != 0:
    print "Error, gcc either had warnings or the code completely failed to compile.  Fix these things before proceeding.  The compiler is your friend, and is trying to help you."
    fail()
    
print "Done."

data=[22,25,23,1,2,3,1,2,3,4,5,9,23,23,12,56,74,1002,34,600,41,44,0,9,3,4,2,3,1,2,3,2,4,23,23,2429023,43,2,32121,12,3]

data_len=[20,18,10,24]

num_procs=[2,1,5,24]

root=[0,0,3,18]

width=150;


for np,dl,r in zip(num_procs,data_len,root):
    print ''.join(['=' for i in range(1,width)])
    for f in glob.glob('whatidid.*'):
        os.remove(f)
    mpicommand=["mpirun","-np",str(np),"--oversubscribe","./"+exename,'--root='+str(r),'--data']+map(str,data[:dl]);
    print "Using the command: "+" ".join(mpicommand)

    print "\n\n\n\n\n"

    P=Popen(mpicommand,stdin=PIPE,stdout=PIPE,stderr=PIPE)
    timer=Timer(timeout,lambda p:p.kill(),[P])

    timer.start()
    P.wait()
    [s1,s2]=P.communicate();
    
    if timer.isAlive():
        timer.cancel()
    else:
        print "It looks like this code entered an infinite loop, or was stuck waiting for input, or just refused to exit.  I had to kill it manually. "
        fail()

    print s1
    print s2

    if P.returncode != 0:
        print "Error.  The return code was non-zero, meaning that this code crashed."
        fail()


    for p in range(0,np):
        L=open("whatidid.rank."+str(p)+".txt").readlines();
        L=[l for l in L if 'No'  not in l];
        num_sends=int(L[0].split(':')[1])
        if p==r:
            targ_sends=np-1
        else:
            targ_sends=0
        if num_sends != targ_sends:
            print "Error, rank "+str(p)+" should have done "+str(np-1)+" sends, but it did "+str(num_sends)
        final_data=map(int,L[-1].split(':')[1].strip().strip('[]').split(','))
        if final_data!=data[:dl][dl/np*p:dl/np*(p+1)]:
            print "Error, rank "+str(p)+" should have ended up with "+str(data[:dl][dl/np*p:dl/np*(p+1)])+" but it ended up with "+str(final_data)
            fail()
            
succeed()
