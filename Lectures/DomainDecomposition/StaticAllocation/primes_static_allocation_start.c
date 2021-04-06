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
  for(i=2;i<n;i++) {
    if (n%i==0){
      return 0;
    }
  }

  return i;
}


void send_results(int* results, int count) 
{
  MPI_Send(results,count,MPI_INT,0,0,MPI_COMM_WORLD);
 
}

void recv_results(int rank,int* results, int bfsize, int* num_recvd) 
{
  MPI_Status status;
  MPI_Recv(results,bfsize,MPI_INT,rank,0,MPI_COMM_WORLD,&status); 
  MPI_Get_count(&status,MPI_INT,num_recvd); 
}

void workerstuff(options* o) 
{
  // Figure out which numbers I am responsible for
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  int total_num_to_check = o->maxn - 1;
  int lower_bounds[o->numprocs];
  int upper_bounds[o->numprocs];

  int chunk = total_num_to_check/o->numprocs;
  int i;

  // Figure out what everyone is responsible for
  lower_bounds[0] = 2;
  for(i = 0;i < o->numprocs;i++)
  {
    upper_bounds[i] = lower_bounds[i] + chunk - 1;
    if (i < (total_num_to_check % o->numprocs))
    {
      upper_bounds[i]++;
    }
    if (i < o->numprocs - 1)
    {
      lower_bounds[i+1] = upper_bounds[i] + 1;
    }
  }


  int my_lower_bound = lower_bounds[rank];
  int my_upper_bound = upper_bounds[rank];
  int my_num_to_check = my_upper_bound - my_lower_bound;

  printf("Rank %d checking [%d, %d]\n",rank,my_lower_bound,my_upper_bound);

  int* my_primes=malloc(my_num_to_check/2*sizeof(int));
  int my_prime_count = 0;
  for (i = my_lower_bound;i <= my_upper_bound;i++)
  {
    if (is_prime(i))
    {
      my_primes[my_prime_count] = i;
      my_prime_count++;
    }
  }

  // Send Results
  send_results(my_primes,my_prime_count);

  free(my_primes);
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

  // Store our results into the total array
  int *final_prime_list = malloc(total_num_to_check/2*sizeof(int));
  int final_prime_count = my_prime_count;

  for(int i = 0;i < my_prime_count;i++)
  {
    final_prime_list[i] = my_primes[i];
  }

  // Get results from workers
  int buff_size = my_num_to_check/2;
  int* results_buffer = malloc(buff_size*sizeof(int));
  int received_from_buffer = 0;
  int actual_buffsize;

  for(int i = 1;i < o->numprocs;i++)
  {
    recv_results(i,results_buffer,buff_size,&actual_buffsize);
    for(int j = 0;j < actual_buffsize;j++)
    {
      final_prime_list[final_prime_count] = results_buffer[j];
      final_prime_count++;
    }
  }

  print_int_arr(final_prime_list,final_prime_count);
  free(my_primes);
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
