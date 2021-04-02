#!/usr/bin/env python2

import subprocess
import re
from subprocess import Popen, PIPE
from threading import Timer
import time
from math import pi
import math
import os
from shutil import copyfile


RED='\033[91m'
GREEN='\033[92m'
ENDC='\033[0m'

def fail():
    print "This code is "+RED+" not acceptable"+ENDC
    exit(1)

def succeed():
    print "This code is "+GREEN+"acceptable"+ENDC
    exit(0)


sourcename="Buffon.c"
exename="Buffon"

required_files=['more_test_helpers.c']
standard_path='../../standard_code'

for f in required_files:
    if not os.path.isfile(f):
        print("copying "+f)
        copyfile(standard_path+'/'+f,'./'+f);


mpicccommand=["mpicc","-Wl,-wrap,srand","-Wl,-wrap,MPI_Finalize", "-Wl,-wrap,MPI_Init", sourcename, "-lm", "more_test_helpers.c","-o",exename]
print "Compiling your "+sourcename+" file ...."
print " ".join(mpicccommand)
subprocess.call(mpicccommand)
print "Done.\n\n"


num_procs=[2,10,20];
N=[10000,500,100000]

for testno in range(len(num_procs)):
    pi_est=0;
    
    mpicommand=["mpirun","-np",str(num_procs[testno]),'--oversubscribe',"./"+exename,str(N[testno])]
    print "Launching using the command: "+" ".join(mpicommand)
    

    print "\n\n\n"

    P=Popen(mpicommand,stdin=PIPE,stdout=PIPE,stderr=PIPE,bufsize=0)

    timer=Timer(60,lambda p:p.kill(),[P])
    timer.start()

    P.wait();
    [s1,s2]=P.communicate();
    if timer.isAlive():
        timer.cancel()
    else:
        print "It looks like this code entered an infinite loop, or was stuck waiting for input, or just refused to exit.  I had to kill it manually. "
        fail();

    pi_est=(float(re.findall('\d\.\d+',s1)[0]));
    seeds=[]
    print("The estimate of pi generated is: "+str(pi_est))
    for procno in range(num_procs[testno]):
        lines=open("whatidid.rank."+str(procno),"r").readlines();
        seedtimes=int(lines[0].split(":")[1]);
        if seedtimes != 1:
            print "Error: Processor "+str(procno)+" should seed the random number generator exactly 1 time, but it seeded it "+str(seedtimes)+" times"
            fail()
    	seeds.append(int(lines[1].split(':')[1]));
    	#print("seeds:"+str(seeds))
    	for k in range(len(seeds)):
        	if seeds.count(seeds[k])>1:
        		print("Error.  More than one processor seeded the random number generator with the seed "+str(seeds[k])+".  This will produce biased samples.")
        		fail()
    	linecross_calls=int(lines[2].split(':')[1]);
        if linecross_calls != N[testno]:
            print "Error.  Processor "+str(procno)+" should have tested "+str(N[testno])+" samples, but it tested "+str(linecross_calls)
            fail()
    num_samples=N[testno]*num_procs[testno];
    sigma=1.0/num_samples**0.5*((math.pi-1)/math.pi**2)**0.5;
    diff=abs(1/pi_est-1/math.pi)
    if diff>3*sigma:
    	print("This estimate of pi is far too poor for this many samples. There is likely a bug in your code.")
    	fail()

succeed()
