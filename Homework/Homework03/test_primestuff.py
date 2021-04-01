#!/usr/bin/env python2
import sys
import os
import runlib
import testlib
import glob
class HW2Tester:
    def __init__(self, sourcename='', sourcefiles=[],helper_sourcefiles=[],exename=None,ref_dir='./',files_dir='',cl_arguments=[],max_tests=0):
        self.sourcename=sourcename;
        if exename is None:
            exename='./'+self.sourcename;
        self.exename=exename;
        self.sourcefiles=sourcefiles;
        self.helper_sourcefiles=helper_sourcefiles;
        self.max_tests=max_tests;
        
        #This is the directory in which all helpers and data files live,
        self.ref_dir=ref_dir
        self.files_dir=files_dir
        #process any command line arguments
        for a in cl_arguments:
            if a.find('--ref-dir=')==0:
                self.ref_dir=a.split('=')[1]
            elif a.find('--max-procs=')==0:
                try:
                    self.max_procs=int(a.split('=')[1])
                except ValueError:
                    print("Error, you must provide an integer number of processors with --max-procs=")
                    exit(1)
            elif a.find('--files-dir=')==0:
                self.files_dir=a.split('=')[1]
            else:
                print("Error, option "+a+" not supported")
                exit(1)
                

    #Takes the common options and runs.  Checks for failure and prints a message if the code crashes.
    #returns the stdout and stderr of the command.  
    def run_instance(self,standard,options=None,outputfile=True,timeout=30):
        if not standard:
            for f in glob.glob('whatidid.*'):
                os.remove(f)
        if standard==False:
            command=[self.exename]
        else:
            command=[self.ref_dir+self.exename+'_standard']
        returncode,s1,s2=runlib.run_command(command=command+options,timeout=timeout)
        if returncode!=0:
            print("\n"+testlib.RED+"Error, the command crashed. "+testlib.ENDC+"Here is the output:\n\n")
            print(s1)
            print(s2)
            testlib.fail()
        return s1,s2
                     

    def check_output_correctness(self,output1,output2):
        return "".join(output1.split()).lower() == "".join(output2.split()).lower()
        

    def testcase(self, case, timeout=2):
        com=[];
        com.append(["--list","1","20"])
        com.append(["--list","-1","0","1", "2", "3", "4", "5", "6", "7", "8", "9", "10"])
        com.append(["--range", "5","20"])
        com.append(["--range"])

        com.append(["--notanoption"])
        com.append(["--range","1"])
        com.append(["--range","1" ,"500"])    

        self.separator()
        print("Testcase "+str(case)+"\n");
        s1,s2=self.run_instance(standard=False,options=com[case-1],timeout=timeout);
        
        s1_stand,s2_stand=self.run_instance(standard=True,options=com[case-1],timeout=timeout);
        
        return self.check_output_correctness(s1,s1_stand),s1,s2,s1_stand,s2_stand;
                                      
    def separator(self):
        width=100
        print("".join(['=' for i in range(width)]));
    
        
    def dotest(self):
        #compile up the student executable. 
        testlib.compile(gcc='gcc',flags=['-lm'],log_mpi=False,sourcefiles=self.sourcefiles+self.helper_sourcefiles,exename=self.exename);

        prob_correct=True;
        for i in range(1,self.max_tests+1):
            correct,stdout,stderr,stdout_stand,stderr_stand=self.testcase(i);
            if correct == False:
                prob_correct=False
                print("There was an error in this testcase, the output of the standard code and submitted code did not match.")
                print(testlib.BLUE+"Standard code:\n"+testlib.ENDC)
                print(stdout_stand);
                print(testlib.BLUE+"Submitted code:\n"+testlib.ENDC);
                print(stdout);
                print('Testcase'+str(i)+':['+testlib.RED+'FAIL'+testlib.ENDC+']');
            else:
                print('Testcase '+str(i)+':['+testlib.GREEN+'OK'+testlib.ENDC+']');
        if not prob_correct:
            testlib.fail()
            sys.exit(0);
            
        else:
            testlib.succeed();
            sys.exit(1);



PA=HW2Tester(sourcename='primestuff',sourcefiles=['primestuff.c'],max_tests=7);
            
if __name__=='__main__':
    PA.dotest()
