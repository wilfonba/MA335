#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <float.h>

#include "goldbach_helpers.c"

int goldbach_partition_calls=0;

//returns 1 if n is prime, 0 otherwise
int isprime(int n){
  if (n<2){
    return 0;
  }
  int i=2;
  while (i*i<=n){
    if (n%i==0){
      return 0;
    }
    i++;
  }
  return 1;
}



//Attempts to calculate a Golbach partition of n.
//Returns a Goldbach partition that includes the smallest possible prime.
//i.e., both 3/7 and 5/5 are partitions of 10.  3/7 is returned because 3 is the smallest of 3,5,7.
//Returns 1 on success, in this case a and b hold the partition
//Returns 2 if n<=2, or if n is odd
//Returns 3 if you have disproved Golbach's conjecture. 
int goldbach_partition(int n, int* a, int* b){
  goldbach_partition_calls++;
  int exit_code=2;
  if (n<=2 || (n % 2 ==1)){
    return exit_code;
  }

  exit_code=3;

  int i;
  for (i=2;i<=n/2;i++){
    if (isprime(i) && isprime(n-i)){
      *a=i; *b=n-i;
      exit_code=1;
      return exit_code;
    }
  }

  return exit_code;
 
}

int min(int a, int b){
  return a<b ? a:b;
}


int main(int argc, char** argv){

  MPI_Init(&argc,&argv);

  
  int rank;
  int size;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  
  options opt;
  parse_options(argc,argv,&opt);
  if (rank==0){
    print_options(&opt);
  }

  //The command line options have now been parsed properly.
  //They are held in the members of opt.
  //The options you care about are:
  //opt.print_results  -- 0 if you should not print out the partitions you find, 1 if you should
  //opt.to_partition   -- the list of numbers that you should find partitions for
  //opt.num_to_partition -- the length of the list above
  //opt.chunksize -- the control for the "chunksize" of a work assignment.  If the chunksize is n, then processors get assigned to partition n consecutive numbers from the to_partition list as part of 1 assignement. 
  
  
  MPI_Finalize();


}
