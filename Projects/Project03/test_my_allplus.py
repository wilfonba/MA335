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


ref_dir='./'
if len(sys.argv)>1:
    ref_dir=sys.argv[1]+'/'

    
sourcename="my_allplus.c"
exename="my_allplus_exe"
timeout=60

mpicccommand=["mpicc","--std=c99",'-g','-lm','-Wl,-wrap,MPI_Recv',"-Wl,-wrap,MPI_Send", "-Wl,-wrap,main", sourcename, ref_dir+"my_allplus_test_driver.c",ref_dir+"test_helpers.c",ref_dir+"mpi_send_wrapper.c","-o",exename]
print "Compiling your "+sourcename+" file ...."
print " ".join(mpicccommand)
gcc_status=subprocess.call(mpicccommand)
if gcc_status != 0:
    print "Error, gcc either had warnings or the code completely failed to compile.  Fix these things before proceeding.  The compiler is your friend, and is trying to help you."
    fail()
    
print "Done."

random.seed(1)
data=[random.randint(1,50) for i in range(500)];

data_len=[4,18,25,6,49,49,150,255,32,300]

num_procs=[2,3,5,6,7,7,3,15,16,1]

width=150;

for np,dl in zip(num_procs,data_len):
    print ''.join(['=' for i in range(1,width)])
    
    for f in glob.glob('whatidid.*'):
        os.remove(f)
    mpicommand=["mpirun","-np",str(np),"--oversubscribe","./"+exename,'--data']+map(str,data[:dl]);
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
        sends_line=[i for i in range(len(L)) if 'total sends' in L[i]][0];
        recvs_line=[i for i in range(len(L)) if 'total recvs' in L[i]][0];
        fd_line=[i for i in range(len(L)) if 'Final' in L[i]];
        if len(fd_line)>0:
            fd_line=fd_line[0]
        else:
            fd_line=len(L)
        num_sends=int(L[sends_line].split(':')[1]);
        num_recvs=int(L[recvs_line].split(':')[1]);

        send_data=[map(int,L[i].split()[2].strip().strip('[]').split(',')) for i in range(sends_line+2,recvs_line)]
        recv_data=[map(int,L[i].split()[2].strip().strip('[]').split(',')) for i in range(recvs_line+1,fd_line)]

        final_data=map(int,L[fd_line].split(':')[1].strip().strip('[]').split(','))

        if num_sends > ceil(log(np,2)):
            print "Error! Rank "+str(p)+" should do no more than "+str(int(log(np,2)))+" sends, but it did "+str(num_sends)
            fail()
        if num_recvs > ceil(log(np,2)):
            print "Error! Rank "+str(p)+" should do no more than "+str(int(log(np,2)))+" recieves, but it did "+str(num_recvs)
            fail()
        if num_sends > 0 and max([len(x) for x in send_data])>1:
            print "Error! Rank "+str(p)+" did a send that was too long, it was "+str(max([len(x) for x in send_data]))+" integers long."
            fail()
        if num_recvs > 0 and max([len(x) for x in recv_data])>1:
            print "Error! Rank "+str(p)+" did a receive that was too long, it was "+str(max([len(x) for x in recv_data]))+" integers long."
            fail()
        
        if final_data[0] != sum(data[:dl]):
            print "Error! Rank "+str(p)+" should have ended up with sum "+str(sum(data[:dl]))+", but it ended up with "+str(final_data)
            fail()
            
succeed()
