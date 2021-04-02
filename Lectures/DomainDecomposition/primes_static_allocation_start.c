#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "helpers_static.h"


int is_prime(int n) {
  int i;
  if(n<=1) {
    return 0;
  }
  for(i=2;i*i<=n;i++) {
    if (n%i==0){
      return 0;
    }
  }

  return i;
}


void send_results(int* results, int min, int max) {

 
}

void recv_results(int rank,int* results, int bfsize) 
{
  MPI_Status status;
  MPI_Recv(results,bfsize,MPI_INT,rank,0,MPI_COMM_WORLD,&status);  
}

void workerstuff(options* o) {

}


void masterstuff(options* o) {
  int total_num_to_check = o->maxn - 1;
  int my_num_to_check = total_num_to_check/o->numprocs;
  int my_lower_bound,my_upper_bound;

  if (total_num_to_check % (o->numprocs))
  {
    my_num_to_check++;
  }

  my_lower_bound = 2;
  my_upper_bound = my_lower_bound + my_num_to_check - 1;
  printf("Rank 0 checking [%d, %d] \n",my_lower_bound,my_upper_bound);

  // Variables to store primes and how many there are
  int* my_primes = malloc(my_num_to_check/2*sizeof(int));
  int my_prime_count = 0;

  for(int i = my_lower_bound;i <= my_upper_bound;i++)
  {
    if (is_prime(i))
    {
      my_primes[my_prime_count] = i;
      my_prime_count++;
    }
  }
  printf("Rank 0 found primes ");
  print_int_arr(my_primes,my_prime_count);

  // Store our results into the total array
  int *final_prime_list = malloc(total_num_to_check/2*sizeof(int));
  int final_prime_count = my_prime_count;

  for(int i = 0;i < my_prime_count;i++)
  {
    final_prime_list[i] = my_primes[i];
  }

  // Get results from workers
  int buff_size = y_num_to_check/2*;
  int* results_buffer = malloc(buff_size*msizeof(int));
  int received_from_buffer = 0;

  for(int i = 0;i < 0->numprocs;i++)
  {
    recv_results(i,results_buffer,buff_size);
    for(int j = 0;j < )
  }
}


int main(int argc, char** argv) {
  MPI_Init(&argc,&argv);
  int rank;
  int numproc;

  MPI_Comm_size(MPI_COMM_WORLD,&numproc);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  
  options o;
  set_opts(argc,argv,&o);
  if (rank==0){
    print_opts(&o);
    masterstuff(&o);
  } else {
    workerstuff(&o);
  }
  
  MPI_Finalize();
  return 0;
}
