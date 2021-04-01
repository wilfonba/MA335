#!/usr/bin/env python2
import sys
import os
import HW2Tester

PA=HW2Tester.HW2Tester(sourcename='triangles',sourcefiles=['triangles.c'],max_tests=3);
            
if __name__=='__main__':
    sys.exit(PA.dotest())
