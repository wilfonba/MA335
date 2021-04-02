#!/usr/bin/env python2

import subprocess
from subprocess import Popen, PIPE
from threading import Timer
import sys
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


    
sourcename="broadcast_messages.c"
exename="broadcast_messages"

ref_dir='./'
if len(sys.argv)>1:
    ref_dir=sys.argv[1]+'/'

required_files=['some_test_helpers.c']
standard_path='../../standard_code'

for f in required_files:
    if not os.path.isfile(f):
        print("copying "+f)
        copyfile(standard_path+'/'+f,'./'+f);


    
mpicccommand=["mpicc","-Wl,-wrap,MPI_Recv","-Wl,-wrap,MPI_Finalize", sourcename, ref_dir+"some_test_helpers.c","-o",exename]
print "Compiling your "+sourcename+" file ...."
print " ".join(mpicccommand)
subprocess.call(mpicccommand)
print "Done."

data=[4,5,0,-6];

num_procs=6

mpicommand=["mpirun","-np",str(num_procs),'--oversubscribe',"./"+exename];
print "Using the command: "+" ".join(mpicommand)

print "Testing by inputting "+" followed by ".join(map(str,data))
print "\n\n\n\n\n"


P=Popen(mpicommand,stdin=PIPE,stdout=PIPE,stderr=PIPE)

timer=Timer(5,lambda p:p.kill(),[P])

timer.start()

[s1,s2]=P.communicate(input='\n'.join(map(str,data))+'\n');
if timer.isAlive():
    timer.cancel()
else:
    print "It looks like this code entered an infinite loop, or was stuck waiting for input, or just refused to exit.  I had to kill it manually. "
    fail()


print s1
print s2

result=[1 for x in range(num_procs)];
for i in range(1,num_procs):
    R=open("whatidid.rank."+str(i)).readlines();
    R=R[1:];
    if len(R) != len(data):
        fail();
        result[i]=0
    for j in range(len(data)):
    	actual=map(int,R[j].split())[:-1]
    	if actual!=[j,0,data[j]]:
	   result[i]=0
	   print "Error, on rank "+str(i)+" the "+str(j)+"th recieve should come from rank 0 and contain data "+str(data[j])
	   print "However, it was from rank "+str(actual[1])+" and contained data "+str(actual[2])
           fail();
           
if all(result):
    succeed();
else:
    fail();
