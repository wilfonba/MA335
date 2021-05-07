#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<string.h>

#include "helpers.h"
#include  "mergesort.c"

//Bucketsort -- in serial. 


void fill_buckets(double* L, int n, int a, int b, double** B, int m, int bucket_capacity, int* bucket_len){

  //Set the length of each bucket to 0. 
  memset(bucket_len,0,m*sizeof(int));

  //Calculate the width of each bucket. 
  double deltax=1.0*(b-a)/m;

  double val;
  int bucket_idx;
  int i;
  for (i=0;i<n;i++){
    val=L[i];
    //Figure out which bucket val goes in. 
    bucket_idx=floor((val-a)/deltax);
    if (bucket_len[bucket_idx] == bucket_capacity){
      fprintf(stderr,"Oh no.  Bucket %d already contains %d elements, making it full.  Re-run with bigger buckets, or get better balanced lists.\n",bucket_idx,bucket_capacity);
      exit(1);
    }
    //Stick val on the end of B[bucket_idx]
    B[bucket_idx][bucket_len[bucket_idx]]=val;
    //Now increment. 
    bucket_len[bucket_idx]++;
  }



} 


void bucketsort(double* L, int n, int m, int a, int b,options o){

  int i;
  /*Each "bucket" should be an array capable of holding numbers in it's assigned range*/
  /*I want an array of arrays, so a double ** */

  /*Allocate room for m double pointers.  Each one of those will point to an array that is large 
    enough to hold n/m*o.bucket_size_muliplier doubles*/
  double** B=malloc(m*sizeof(double*));
  int bucket_capacity=ceil((1.0*n/m)*o.bucket_size_multiplier);
  /*Ok, since B is a double**, B[i] is a double*, we point each B[i] to a chunk of memory that is
    bucket_capacity large*/
  for (i=0;i<m;i++){
    B[i]=malloc(bucket_capacity*sizeof(double));
  }

  /*We need to store how many things are actually being stored in each bucket*/
  int* bucket_len=malloc(m*sizeof(int));

  /*fill up the buckets, B, of which there are m, with the contents of L, whchi is n long, and is 
    uniformly distributed between a and b.  Each bucket has a capacity of bucket_capacity, please
    fill up the bucket_len array*/
  fill_buckets(L,n,a,b, B, m, bucket_capacity,bucket_len);

  /*Print out each bucket, if desired.  This is purely for fun. */
  int show_buckets=1;
  if (show_buckets){
    double delta=(b-a)/m;
    for (i=0;i<m;i++){
      printf("Bucket: %d range:[%.2lf, %.2lf] contents: %s\n", i,a+i*delta,a+(i+1)*delta,dbl_arr2str(B[i],bucket_len[i]));
    }
  }


  //Keep track of how many entries in L are filled up. 
  int nsorted=0;

  /*Sort each bucket using mergesort. */
  for (i=0;i<m;i++){
    mergesort(B[i],bucket_len[i]);
    if (show_buckets){
      printf("Sorted Bucket: %d contents: %s\n",i,dbl_arr2str(B[i],bucket_len[i]));
    }
    //B[i] has now been sorted.  Copy it onto the end of L. 
    memcpy(L+nsorted,B[i],bucket_len[i]*sizeof(double));
    nsorted+=bucket_len[i];
  }

  for (i=0;i<m;i++){
    free(B[i]);
  }
  free(B);
  free(bucket_len);

}

//Performs bucketsort 
int main(int argc, char** argv){
  MPI_Init(&argc,&argv);
  options o;

  /*Reads input opts from the command line and populates the options struct o*/
  /*The file helpers.h defineds the struct, and the help function directly below
    gives information on what each option is intended to do.*/
  read_options(argc,argv,&o);

  print_options(&o);

  /*L is the unsorted list.  Depending on what the user wants, it is either populated from a file,
    or it is generated randomly*/
  double* L;
  
  if (o.n==-1){
    read_inputfile(o.inputfile,&L,&o);
  }
  else{
    make_random_data(&L,&o);
  }

  /*Display the data if it is requested*/
  if (o.print_data){
    printf("L: %s\n",dbl_arr2str(L,o.n));
  }
  
  double start=MPI_Wtime();
  /*bucketsort L.  L is o.n doubles long, use o.m buckets to do the sort, 
    and L is uniformly distributed beetween o.a and o.b*/
  bucketsort(L,o.n,o.m,o.a,o.b,o);
  double end=MPI_Wtime();

  /*Print out the sorted list if the user wishes*/
  if (o.print_data){
    printf("L: %s\n",dbl_arr2str(L,o.n));
  }
  printf("Elapsed time: %f seconds\n",end-start);
  MPI_Finalize();
  return(0);
}