#!/usr/bin/env python2

import subprocess
from subprocess import Popen, PIPE
from threading import Timer
import os, glob
from math import *
import random
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


def lcgarr(a,c,m,x0,l):
    x=[x0];
    for i in range(1,l):
        x.append((a*x[i-1]+c)%m)
    return x

sourcename="my_allgather.c"
exename="my_allgather_exe"
timeout=30

ref_dir="./"
if len(sys.argv)>1:
    ref_dir=sys.argv[1]+'/'

mpicccommand=["mpicc","--std=c99",'-g','-lm','-Wl,-wrap,MPI_Recv',"-Wl,-wrap,MPI_Send", "-Wl,-wrap,main", sourcename, ref_dir+"test_helpers.c",ref_dir+"my_allgather_test_driver.c",ref_dir+"mpi_send_wrapper.c","-o",exename]
print "Compiling your "+sourcename+" file ...."
print " ".join(mpicccommand)
gcc_status=subprocess.call(mpicccommand)
if gcc_status != 0:
    print "Error, gcc either had warnings or the code completely failed to compile.  Fix these things before proceeding.  The compiler is your friend, and is trying to help you."
    fail()
    
print "Done."

random.seed()


data_len=[10,20,280,13*45,128,10,24,9,15000]

num_procs=[2,4,7,13,8,1,3,9,5]
width=150
for np,dl in zip(num_procs,data_len):
    data=[];
    for p in range(np):
        data=data+lcgarr(1103,13243,500,p,dl/np)
    
    print "".join(['=' for i in range(width)]);
    for f in glob.glob('whatidid.*'):
        os.remove(f)
    mpicommand=["mpirun","-np",str(np),"--oversubscribe","./"+exename,"--len="+str(dl)];
    print "Using the command: "+" ".join(mpicommand)

    P=Popen(mpicommand,stdin=PIPE,stdout=PIPE,stderr=PIPE)
    timer=Timer(timeout,lambda p:p.terminate(),[P])
    timer.daemon=True;
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
        sends_line=[i for i in range(len(L)) if 'total sends' in L[i]][0];
        recvs_line=[i for i in range(len(L)) if 'total recvs' in L[i]][0];
        num_sends=int(L[sends_line].split(':')[1]);
        num_recvs=int(L[recvs_line].split(':')[1]);
        send_partners=[int(x.split()[1]) for x in L[sends_line+2:recvs_line]];
        final_data=map(int,L[-1].split(':')[1].strip().strip('[]').split(','))
        targ_recvs=np-1;
        if num_recvs != targ_recvs:
            print "Error! Rank "+str(p)+" should have done "+str(targ_recvs)+" receives, but it did "+str(num_recvs)
            fail()

        targ_sends=np-1;
        if num_sends != targ_sends:
            print "Error! Rank "+str(p)+" should have done "+str(targ_sends)+" sends, but it did "+str(num_sends)
            fail()

        if final_data!=data:
            print "Error! Rank "+str(p)+" should have ended up with the data "+str(data)+ " in its buffer, but it ended up with "+str(final_data)
            fail()
            
succeed()
