#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<string.h>
#include"bucketsort_helpers.h"
#include "mergesort.c"

//#define DEBUG_BUCKET_CONTENTS


//Takes a list L, of length n, whose elements are evenly distributed between a and b, and classify
//the elements.
//The "buckets", held in B, are arrays.
//There are m buckets, B[0], B[1], ... , B[m-1].
//The buckets must be pre-allocated, the capacity of each bucket is held in bucket_capacity.
//After filling, the number of elements in B[i] is returned in bucket_len[i].

void fill_buckets(double* L, int n, double a, double b, double** B, int m, int bucket_capacity, int* bucket_len){
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


void main(int argc, char** argv){
  
  MPI_Init(&argc,&argv);
  int myRank;
  int numProcs;
  MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
  MPI_Comm_rank(MPI_COMM_WORLD,&myRank);

  options opts;


  double* L;     //the list L.  Only the root will ever have a complete copy of this.
  double* subL;  //the sublist of L that I will sort
  int subL_len;  //The length of that sublist

  double start, end;

  //Every process is good to read the options off the command line, because
  //they all have the command line options, thanks to MPI.
  //Eliminates passing around a bunch of options.


  int i;
  if (myRank==0){
    ///////////////////////////////////////////////////////////////////////
    /////                    //////////////////////////////////////////////
    /////   Root STUFF       //////////////////////////////////////////////
    /////                    //////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////
    parse_options(argc,argv,&opts);
    //rank 0 has the initial list.  Either make it randomly or read it from the input file.
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
    //At this point the options can't change anymore.  So it is safe to tell everyone else what the options are.
    MPI_Bcast(&opts.n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.nproc,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.bucket_size_multiplier,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);



    //print_options(&opts);



    printf("Bucket-Sorting %d elements using %d processes.\n",opts.n,opts.nproc);
    //print the initial list if is is nice and small
    if (opts.print_things){
      printf("L: ");
      print_arr(L,opts.n);
    }

    // Send L_i to processor i
    int i;
    int* sendCounts = malloc(numProcs*sizeof(int));
    int* sendIdxs = malloc(numProcs*sizeof(int));
    *(sendIdxs + 0) = 0;
    for (i = 0;i < numProcs;i++) {
      *(sendCounts + i) = opts.n/numProcs;
      if (i < (opts.n % numProcs)) {
        *(sendCounts + i)+=1;
      }
      if (i != 0) {
        *(sendIdxs + i) = *(sendIdxs + i-1) + *(sendCounts + i-1) ;
      }
    }
    double* recvBuff = malloc(*(sendCounts)*sizeof(double));
    MPI_Scatterv(L,sendCounts,sendIdxs,MPI_DOUBLE,recvBuff,*(sendCounts),MPI_DOUBLE,0,MPI_COMM_WORLD);


    // Divide L_i into buckets B_ij
    int bucketCap = ceil((1.0*opts.n/numProcs)*opts.bucket_size_multiplier);
    double** buckets = malloc(numProcs*sizeof(double*));
    int* bucketLength = calloc(numProcs,sizeof(int));
    for (i = 0;i < numProcs;i++) {
      //printf("Rank 0 allocating memory for each bucket\n");
      buckets[i] = malloc(bucketCap*sizeof(double));
    }
    fill_buckets(recvBuff,*sendCounts,opts.a,opts.b,buckets,numProcs,bucketCap,bucketLength);
    printf("Root successfuly filled buckets\n");fflush(stdout);
    #ifdef DEBUG_BUCKET_CONTENTS
      for (i = 0;i < numProcs;i++) {
        printf("Rank %d bucket %d contains: ",myRank,i);
        print_arr(*(buckets + i),*(bucketLength + i));
        printf("\n");
      }
    #endif
    
    // Free dynamic memory
    free(sendCounts);
    free(sendIdxs);
    free(recvBuff);
    free(L);
    free(buckets);
    free(bucketLength);
  }
  else {
    ///////////////////////////////////////////////////////////////////////
    /////                    //////////////////////////////////////////////
    /////   WORKER STUFF     //////////////////////////////////////////////
    /////                    //////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////

    // Get options from root
    MPI_Bcast(&opts.n,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.nproc,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.bucket_size_multiplier,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    MPI_Bcast(&opts.b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);


    // Receive L_i
    int recvCount = opts.n/numProcs;
    if (myRank < opts.n/numProcs) {
      recvCount++;
    }
    double* recvBuff = malloc(recvCount*sizeof(int));
    MPI_Scatterv(NULL,0,0,MPI_DOUBLE,recvBuff,recvCount,MPI_DOUBLE,0,MPI_COMM_WORLD);


    // Divide L_i into buckets B_ij
    int bucketCap = ceil((1.0*opts.n/numProcs)*opts.bucket_size_multiplier);
    double** buckets = malloc(numProcs*sizeof(double*));
    int* bucketLength = calloc(numProcs,sizeof(int));
    for (i = 0;i < numProcs;i++) {
      printf("Rank %d allocating memory for each bucket\n",myRank);
      buckets[i] = malloc(bucketCap*sizeof(double));
    }
    fill_buckets(recvBuff,recvCount,opts.a,opts.b,buckets,numProcs,bucketCap,bucketLength);
    printf("other proc Successfully filled buckets\n");fflush(stdout);
    #ifdef DEBUG_BUCKET_CONTENTS
      for (i = 0;i < numProcs;i++) {
        printf("Rank %d bucket %d contains: ",myRank,i);
        print_arr(*(buckets + i),*(bucketLength + i));
        printf("\n");
      }
    #endif




    // Free dynamic memory
    free(recvBuff);
    free(buckets);
    free(bucketLength);
  }



  //At the end, the master prints out the sorted list if necessary, prints the outputfile, and if requested it checks to make sure that the list really is sorted. 
  /*if (myRank==0){
    if (opts.produce_outputfile){
      write_outputfile(opts.outputfile,L,opts.n);
    }
    if (opts.print_things){
       printf("L:");
       print_arr(L,opts.n);
    }
    if (opts.check_results){
      if (is_sorted(L,opts.n)){
	      printf("L is sorted ascending\n");
      }
      else{
	      printf("L is NOT sorted ascending!!!\n");
      }
    }
  }*/
  MPI_Finalize();
}




