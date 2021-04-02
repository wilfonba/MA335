#!/usr/bin/env python2

import subprocess
import re
from subprocess import Popen, PIPE
from threading import Timer
import time
from math import pi
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


sourcename="palindrome.c"
exename="palindrome"


required_files=['most_test_helpers.c','dict.c','2wordpalindromes.txt','1wordpalindromes.txt']
standard_path='../../standard_code'

for f in required_files:
    if not os.path.isfile(f):
        print("copying "+f)
        copyfile(standard_path+'/'+f,'./'+f);


ref_dir='./'
if len(sys.argv)>1:
    ref_dir=sys.argv[1]

mpicccommand=["mpicc","-lm","-Wl,-wrap,MPI_Finalize", sourcename, ref_dir+"most_test_helpers.c","-o",exename]

print "Compiling your "+sourcename+" file ...."
print " ".join(mpicccommand)
subprocess.call(mpicccommand)
print "Done.\n\n"


dictlen=12554

two_word_palindromes=[w.strip() for w in open(ref_dir+'2wordpalindromes.txt','r').readlines() ]
one_word_palindromes=[w.strip() for w in open(ref_dir+'1wordpalindromes.txt','r').readlines() ]

two_word_palindromes=[w for w in two_word_palindromes if ('dictionary is' not in w and 'word combinations' not in w and 'dictfile' not in w and 'numwords' not in w)];

one_word_palindromes=[w for w in one_word_palindromes if ('dictionary is' not in w and 'word combinations' not in w and 'dictfile' not in w and 'numwords' not in w)];

two_word_palindromes=sorted(two_word_palindromes)
one_word_palindromes=sorted(one_word_palindromes)

numwords=[1,1,2,2,2];
num_procs=[1,2,2,5,13];

palindromes=[one_word_palindromes,one_word_palindromes,two_word_palindromes,two_word_palindromes,two_word_palindromes]

for testno in range(len(num_procs)):
    mpicommand=["mpirun","-np",str(num_procs[testno]),"--oversubscribe","./"+exename,"--numwords="+str(numwords[testno]),'--dictfile='+ref_dir+'dictionary_stuff/dict.txt']
    print "Launching using the command: "+" ".join(mpicommand)
    print "\n\n\n"

    P=Popen(mpicommand,stdin=PIPE,stdout=PIPE,stderr=PIPE,bufsize=0)

    timer=Timer(120,lambda p:p.kill(),[P])
    timer.start()

    P.wait();
    [s1,s2]=P.communicate();
    if timer.isAlive():
        timer.cancel()
    else:
        print "It looks like this code entered an infinite loop, or was stuck waiting for input, or just refused to exit.  I had to kill it manually. "
        fail()


    print "Checking to see whether the correct palindromes were generated"
    s1=sorted([w.strip() for w in s1.split('\n') if ('worked for' not in w and 'dictionary is' not in w and 'word combinations' not in w and 'numwords' not in w and 'dictfile' not in w and len(w)>0)])

    if s1==palindromes[testno]:
        print "Yes"
    else:
        print "Error.  The correct palindromes were not generated"
        print "The correct palindromes are:"
        print palindromes[testno]
        print "You generated:"
        print s1
        fail()
    numwordcombos=dictlen**numwords[testno];
    lb_per_proc=int(numwordcombos*1.0/num_procs[testno]*0.9)
    ub_per_proc=int(numwordcombos*1.0/num_procs[testno]*1.1)
    for i in range(num_procs[testno]):
        palindrome_calls=int(open('whatidid.rank.'+str(i),'r').readlines()[0].split(':')[1]);
        if palindrome_calls<lb_per_proc or palindrome_calls>ub_per_proc:
            print "Error: Process "+str(i)+" should check between "+str(lb_per_proc)+" and "+str(ub_per_proc)+" combinations for palindrominess"
            print "Instead, it checked "+str(palindrome_calls)
            fail()
            
succeed()
