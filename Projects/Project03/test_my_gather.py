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


sourcename="my_gather.c"
exename="my_gather_exe"
timeout=60

refdir="./"
if len(sys.argv)>1:
    refdir=sys.argv[1]+'/'

mpicccommand=["mpicc","--std=c99",'-g','-lm','-Wl,-wrap,MPI_Recv',"-Wl,-wrap,MPI_Send", "-Wl,-wrap,main", sourcename, refdir+"my_gather_test_driver.c",refdir+"test_helpers.c",refdir+"mpi_send_wrapper.c","-o",exename]
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

root=[0,1,4,3,5,2,1,10,8,0]

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
        fd_line=[i for i in range(len(L)) if 'Final' in L[i]];
        if len(fd_line)>0:
            fd_line=fd_line[0]
        else:
            fd_line=len(L)
        num_sends=int(L[sends_line].split(':')[1]);
        num_recvs=int(L[recvs_line].split(':')[1]);
        max_allowable_sends=1;
        max_allowable_recvs=ceil(log(np,2))
        recv_partners=[int(x.split()[1]) for x in L[recvs_line+1:fd_line]];
        if p==r:
        	final_data=map(int,L[-1].split(':')[1].strip().strip('[]').split(','));
#        log_p=(p-r+2*np)%np
#        if (log_p==0):
#            recv_target=int(ceil(log(np,2)));
#        else:
#            recv_target=[j for j in range(int(ceil(log(log_p,2)))+1) if log_p % 2**j ==0][-1] 
#        log_targ_recv_partners=[log_p+2**j for j in range(0,recv_target) if log_p+2**j < np]
#        targ_recv_partners=[(x + r + np)%np for x in log_targ_recv_partners]

#        if targ_recv_partners != recv_partners:
#            print "Error! Rank "+str(p)+" should have received data from ranks "+str(targ_recv_partners)+", but instead it received data from "+str(recv_partners)
#            fail()
		if num_recvs>max_allowable_recvs:
			print "Error! Rank "+str(p)+" should have done no more than "+str(max_allowable_recvs)+" recieves, but it did "+str(num_recvs);
			fail()
		if num_sends>max_allowable_sends:
			print "Error! Rank "+str(p)+" should have done no more than "+str(max_allowable_sends)+" sends, but it did "+str(max_allowable_sends);
			fail()
        if p==r and final_data!=data[:dl]:
            print "Error! Rank "+str(p)+" should have ended up with the data "
            print data[:dl]
            print " in its buffer, but it ended up with "
            print final_data

            
            fail()
            
succeed()
