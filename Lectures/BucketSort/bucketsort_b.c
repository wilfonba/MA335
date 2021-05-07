#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<string.h>
#include"helpers.h"
#include "mergesort.c"

/*
//Take a list L, of length n, whose elements are evenly distributed between a and b, and classify
//the elements.
//The "buckets", held in B, are arrays.
//There are m buckets, B[0], B[1], ... , B[m-1].
//The buckets must be pre-allocated, the size of each bucket is held in bucket_capacity.
//After filling, the number of elements in B[i] is returned in bucket_len[i].
*/

void fill_buckets(double* L, int n, double a, double b, double** B, int m, int bucket_capacity, int* bucket_len){


  memset(bucket_len,0,m*sizeof(int));
  double deltax=1.0*(b-a)/m;

  double val;
  int bucket_idx;
  int i;
  for (i=0;i<n;i++){
    val=L[i];
    bucket_idx=floor((val-a)/deltax);
    if (bucket_len[bucket_idx] == bucket_capacity){
      fprintf(stderr,"Oh no.  Bucket %d already contains %d elements, making it full.  Re-run with bigger buckets, or get better balanced lists.\n",bucket_idx,bucket_capacity);
      exit(1);
    }
    B[bucket_idx][bucket_len[bucket_idx]]=val;
    bucket_len[bucket_idx]++;
  }



}

//Filter the elements of L that belong in bucket bucket_num.  Put them in B, and return how many elements there are in B via bucket_len. 


void filter_list(double* L, int n, int a, int b, double* B, int m, int bucket_capacity, int* bucket_len, int bucket_num){
  *bucket_len=0;
  double deltax=1.0*(b-a)/m;
  
  double val;
  int bucket_idx;
  int i;
  for (i=0;i<n;i++){
    val=L[i];
    bucket_idx=floor((val-a)/deltax);
    if (bucket_idx==bucket_num){
      if (*bucket_len == bucket_capacity){
	fprintf(stderr,"Oh no.  Bucket %d already contains %d elements, making it full.  Re-run with bigger buckets, or get better balanced lists.\n",bucket_idx,bucket_capacity);
	exit(1);
      }
      B[*bucket_len]=val;
      (*bucket_len)++;
    }
  }

}




void main(int argc, char** argv){
  
  MPI_Init(&argc,&argv);
  int rank;
  int nproc;
  MPI_Comm_size(MPI_COMM_WORLD,&nproc);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);

  options opts;

  int i;
  if (rank==0){

    read_options(argc,argv,&opts);
    
    //rank 0 has the initial list.  Either make it randomly or read it from the input file.
    double* L;
    if (opts.n==-1){
      read_inputfile(opts.inputfile, &L, &opts);
    } else {
      //seed the random number generator with 1 everytime to get consistent results. 
      srand(1);
      L=malloc(opts.n*sizeof(double));

      //fill up the list with random stuff
      for (i=0;i<opts.n;i++){
        L[i]=((double)rand())/RAND_MAX*(opts.b-opts.a)+opts.a;
      }
    }

    //At this point the options can't change anymore.  So it is safe to tell everyone else what the options are.
    MPI_Bcast(&opts.n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.nproc,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.bucket_size_multiplier,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    
    print_options(&opts);

    printf("Bucket-Sorting %d elements using %d processes.\n",opts.n,opts.nproc);


   //print the initial list if is is nice and small
   if (opts.n<30){
     printf("L: %s\n",dbl_arr2str(L,opts.n));
   }

   double start=MPI_Wtime();

   //ship off L to everyone
   //First everyone needs to know how big L is
   MPI_Bcast(&(opts.n),1,MPI_INT,0,MPI_COMM_WORLD);
   //Now send off L
   MPI_Bcast(L,opts.n,MPI_DOUBLE,0,MPI_COMM_WORLD);
   
   //my bucket
   double* B;
   int bucket_capacity=ceil((1.0*opts.n/opts.nproc)*opts.bucket_size_multiplier);
   int i;
   B=malloc(bucket_capacity*sizeof(double));

   //A place to hold how many elements are actually in my  bucket
   int bucket_len;

   filter_list(L, opts.n, opts.a, opts.b, B, opts.nproc, bucket_capacity, &bucket_len, rank);
 
   
   mergesort(B,bucket_len);

   //now my bucket is good to go!
   memcpy(L,B,bucket_len*sizeof(double));
   
   MPI_Status status;
   int total_nrecvd=bucket_len;
   int nrecvd;
   //get all the sorted buckets back.  It is ok to recieve into L, just keep track of where the data needs to go. 
   for (i=1;i<opts.nproc;i++){
     MPI_Recv(L+total_nrecvd,opts.n-nrecvd,MPI_DOUBLE,i,0,MPI_COMM_WORLD,&status);
     MPI_Get_count(&status,MPI_DOUBLE,&nrecvd);
     total_nrecvd+=nrecvd;
   }


   //All done!
   double end=MPI_Wtime();
   
   //Print L if feasible. 
   if (opts.n<30){
     printf("\n");
     printf("L: %s\n",dbl_arr2str(L,opts.n));
   }
  
   printf("Elapsed time: %f seconds\n",end-start);
   if (opts.check_results){
     if (is_sorted(L,opts.n)){
       printf("L is sorted ascending\n");
     }
     else{
       printf("L is NOT sorted ascending!!!\n");
     }
   }

  }

  else{
    // if you are a worker, your job is easier.
    MPI_Bcast(&opts.n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.nproc,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.bucket_size_multiplier,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

    

    // how big is L
    int L_len;
    MPI_Bcast(&L_len, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    //make the bucket however big the message is. 
    double* L=malloc(L_len*sizeof(double));
    
  
    MPI_Bcast(L,L_len,MPI_DOUBLE,0,MPI_COMM_WORLD);

    //filter my data
    //my bucket
    double* B;
    int bucket_capacity=ceil((1.0*L_len/opts.nproc)*opts.bucket_size_multiplier);
    int i;
    B=malloc(bucket_capacity*sizeof(double));

    //A place to hold how many elements are actually in my  bucket
    int bucket_len;

    filter_list(L, L_len, opts.a, opts.b, B, opts.nproc, bucket_capacity, &bucket_len, rank);
 
   
    //quicksort my bucket
    mergesort(B,bucket_len);

    
    //Send the data on back. 
    MPI_Send(B,bucket_len,MPI_DOUBLE,0,0,MPI_COMM_WORLD);
  }




  MPI_Finalize();

}




