#!/usr/bin/env python2
import sys
import os
import runlib
import testlib

            
if __name__=='__main__':
    testlib.compile(sourcefiles=['randomstuff.c','randomstuff_standard.o','test_randomstuff.o'],exename='test_randomstuff_exe',wrapped_funcs=['main'],log_mpi=False,std_gccflags=['-std=c99','-g','-Wall','-lm'],gcc='gcc')

    runlib.run_command(command=['./test_randomstuff_exe'],print_output=True);
