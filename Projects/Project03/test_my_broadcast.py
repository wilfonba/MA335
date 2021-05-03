#!/usr/bin/env python2

import subprocess
from subprocess import Popen, PIPE
from threading import Timer
import os, glob
from math import *
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


sourcename="my_broadcast.c"
exename="my_broadcast_exe"
timeout=60

ref_dir='./'
if len(sys.argv)>1:
    ref_dir=sys.argv[1]+'/'

mpicccommand=["mpicc","--std=c99",'-g','-lm','-Wl,-wrap,MPI_Recv',"-Wl,-wrap,MPI_Send", "-Wl,-wrap,main", sourcename, ref_dir+"my_broadcast_test_driver.c",ref_dir+"test_helpers.c",ref_dir+"mpi_send_wrapper.c","-o",exename]
print "Compiling your "+sourcename+" file ...."
print " ".join(mpicccommand)
gcc_status=subprocess.call(mpicccommand)
if gcc_status != 0:
    print "Error, gcc either had warnings or the code completely failed to compile.  Fix these things before proceeding.  The compiler is your friend, and is trying to help you."
    fail()
    
print "Done."

data=[22,25,23,1,2,3,1,2,3,4,5,9,23,23,12,56,74,1002,34,600,41,44,0,9,3,4,2,3,1,2,3,2,4,23,23,2429023,43,2,32121,12,3]

data_len=[1,2,20,18,10,24]

num_procs=[2,4,8,16,64,1]

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
        num_sends=int(L[sends_line].split(':')[1]);
        num_recvs=int(L[recvs_line].split(':')[1]);
        send_partners=[int(x.split()[1]) for x in L[sends_line+2:recvs_line]];
        final_data=map(int,L[-1].split(':')[1].strip().strip('[]').split(','))
        if p==0:
            targ_recvs=0
        else:
            targ_recvs=1
        if num_recvs != targ_recvs:
            print "Error! Rank "+str(p)+" should have done "+str(targ_recvs)+" receives, but it did "+str(num_recvs)
            fail()

        if p==0:
            targ_sends=int(ceil(log(np,2)))
        else:
            targ_sends=int(ceil(log(np,2)-floor(log(p,2))-1))
        if num_sends != targ_sends:
            print "Error! Rank "+str(p)+" should have done "+str(targ_sends)+" sends, but it did "+str(num_sends)
            fail()

        if p==0:
            targ_send_partners=[2**k for k in range(0,int(ceil(log(np,2))))]
        else:
            targ_send_partners=[p+2**(k+floor(log(p,2))) for k in range(1,targ_sends+1)]
        if set(targ_send_partners) != set(send_partners):
            print "Error! Rank "+str(p)+" should have send data to ranks "+str(targ_send_partners)+", but instead it send data to "+str(send_partners)
            fail()
        if final_data!=data[:dl]:
            print "Error! Rank "+str(p)+" should have ended up with the data "+str(data[:dl])+ " in its buffer, but it ended up with "+str(final_data)
            fail()
            
succeed()
