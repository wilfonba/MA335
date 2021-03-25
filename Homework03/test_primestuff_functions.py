#!/usr/bin/env python2
import sys
import os
import runlib
import testlib

            
if __name__=='__main__':
    testlib.compile(sourcefiles=['primestuff.c','primestuff_standard_funcs.o','test_primestuff_functions.o'],exename='test_primestuff_functions_exe',wrapped_funcs=['main'],log_mpi=False,std_gccflags=['-std=c99','-g','-Wall','-lm'],gcc='gcc')

    runlib.run_command(command=['./test_primestuff_functions_exe'],print_output=True);
