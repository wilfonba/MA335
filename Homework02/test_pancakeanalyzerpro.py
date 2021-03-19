#!/usr/bin/env python2
import sys
import os
import HW2Tester

PPA=HW2Tester.HW2Tester(sourcename='pancakeanalyzerpro',sourcefiles=['pancakeanalyzerpro.c'],max_tests=3);
            
if __name__=='__main__':
    sys.exit(PPA.dotest())
