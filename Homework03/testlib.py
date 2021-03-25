import runlib
import sys

RED='\033[91m'
GREEN='\033[92m'
BLUE='\033[94m'
CYAN='\033[96m'
ENDC='\033[0m'


#prints the standard non-acceptable message with coloring, and exits with exit code 1
def fail():
    print("This code is "+RED+" not acceptable"+ENDC)
    sys.exit(1)

#prints the standard acceptable message with coloring, and exists with code 0
def succeed():
    print("This code is "+GREEN+"acceptable"+ENDC)
    sys.exit(0)

#compiles some code
#The sourcefiles are the source or object files to be compiled into an executeable,
#the exename is the name of the executable produced.
#std_gccflags are the compile flags -- they default to -std=c99, -g, and -Wall
#std_wrapped_funcs -- the funcs to be wrapped.  Defaults to all the functions wrapped in mpi_wrappers.c, so by default we enable mpi logging.
#wrapped_funcs -- functions to wrap that are not covered by std_wrapped_funcs.  Defaults to [].  The reason to have two separate oions is so that you can easily wrap another function without needing to relist all the stuff in std_wrapped_funcs.  E.g., if I want to wrap in_circle in addition to all the mpi stuff, I levae std_wrapped_funcs alone and just pass wrapped_funcs=['in_circle']
#
#This code will exit and state that the code is not acceptable if compilation fails. 
def compile(sourcefiles=[],exename='',flags=[],gcc='mpicc',std_gccflags=['-std=gnu99','-g','-Wall'],wrapped_funcs=[],log_mpi=True):

    if log_mpi:
        wrapped_funcs=wrapped_funcs+['MPI_Finalize','MPI_Send','MPI_Recv','MPI_Allgatherv','MPI_Alltoall','MPI_Scatter','MPI_Gather','MPI_Allgather','MPI_Alltoallv','MPI_Scatterv','MPI_Gatherv','MPI_Isend','MPI_Sendrecv']
        sourcefiles=sourcefiles+['logger.c','mpi_wrappers.c']

    command=[gcc]+flags+std_gccflags+map(lambda x:'-Wl,-wrap,'+x,wrapped_funcs)+sourcefiles+['-o']+[exename]
    stat=runlib.run_command(command=command,print_output=True)[0];
    if stat!=0:
        print("Error, compilation failed or had a warning.  Fix those before proceeding.")
        fail()
        
