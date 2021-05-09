#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<string.h>
#include"helpers.h"
#include"mergesort.c"


/*
//Take a list L, of length n, whose elements are evenly distributed between a and b, and classify
//the elements.
//The "buckets", held in B, are arrays.
//There are m buckets, B[0], B[1], ... , B[m-1].
//The buckets must be pre-allocated, the size of each bucket is held in bucket_capacity.
//After filling, the number of elements in B[i] is returned in bucket_len[i].
*/

void fill_buckets(double* L, int n, int a, int b, double** B, int m, int bucket_capacity, int* bucket_len){


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


void worker_stuff(options opts){

    // if you are a worker, your job is easier. 
    MPI_Bcast(&opts.n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.nproc,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.bucket_size_multiplier,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

    //Here is a fun trick.  See how big a message is before you get it.
    MPI_Status status;
    MPI_Probe(0,0,MPI_COMM_WORLD,&status);

    //make the bucket however big the message is. 
    int bucket_capacity;
    MPI_Get_count(&status,MPI_DOUBLE,&bucket_capacity);

    
    double* bucket=malloc(bucket_capacity*sizeof(double));

    //Get the bucket from the master.
    MPI_Recv(bucket,bucket_capacity,MPI_DOUBLE,0,0,MPI_COMM_WORLD,&status);
    //Sort my data
    mergesort(bucket,bucket_capacity);
    //Send the data on back. 
    MPI_Send(bucket,bucket_capacity,MPI_DOUBLE,0,0,MPI_COMM_WORLD);

    free(bucket);
}



void master_stuff(options opts,double* L){

   /*Broadcast the options out.  We could have had each processor read the options from the 
      command line just as well*/
    MPI_Bcast(&opts.n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.nproc,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.bucket_size_multiplier,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);

   double start=MPI_Wtime();

   //An array of buckets.  Notice that I'm ignoring the option for number of buckets, and just
   //using all my processors. 
   double* B[opts.nproc];
   int bucket_capacity=ceil((1.0*opts.n/opts.nproc)*opts.bucket_size_multiplier);
   int i;
   for (i=0;i<opts.nproc;i++){
     B[i]=malloc(bucket_capacity*sizeof(double));
   }

   //A place to hold how many elements are actually in each bucket
   int bucket_len[opts.nproc];
   
   //put L into buckets
   fill_buckets(L,opts.n,opts.a,opts.b, B, opts.nproc, bucket_capacity,bucket_len);
   
   //ship the buckets off for sorting. 
   //I'm not using MPI_Scatter for a few reasons:
     // a) The data is not stored in one array in memory -- I would need to copy the buckets
    //      into one array if I wanted to use MPI_Scatter
     // b) I don't want to send the exact same amount of data to each processor. I could use
     //     MPI_Scatterv for this purpose. 
    
   for (i=1;i<opts.nproc;i++){
     MPI_Send(B[i],bucket_len[i],MPI_DOUBLE,i,0,MPI_COMM_WORLD);
   }

   //mergesort my bucket
   mergesort(B[0],bucket_len[0]);
   
   MPI_Status status;

   //Again, would need a Gatherv if I wanted to use a built in collective.  
   //get all the sorted buckets back.  It is ok to recieve into B[i], because who cares what the old, unsorted data is?  Could do this with gatherv, feeling lazy. 
   for (i=1;i<opts.nproc;i++){
     MPI_Recv(B[i],bucket_len[i],MPI_DOUBLE,i,0,MPI_COMM_WORLD,&status);
   }


   int nsorted=0;
   
   //copy the data out of the sorted buckets over top of L.
   for (i=0;i<opts.nproc;i++){
     memcpy(L+nsorted,B[i],bucket_len[i]*sizeof(double));
     nsorted+=bucket_len[i];
   }

   //All done!
   double end=MPI_Wtime();
   
   //Print L if feasible. 
   if (opts.print_data){
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
    }
    else{
      //seed the random number generator with 1 everytime to get consistent results. 
      srand(1);
      L=malloc(opts.n*sizeof(double));

      //fill up the list with random stuff
      for (i=0;i<opts.n;i++){
      	L[i]=((double)rand())/RAND_MAX*(opts.b-opts.a)+opts.a;
      }
    }

    print_options(&opts);
    printf("Bucket-Sorting %d elements using %d processes.\n",opts.n,opts.nproc);

    master_stuff(opts,L);

  }
 
  else{
    worker_stuff(opts);

  }




  MPI_Finalize();

}




