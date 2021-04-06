typedef struct options{
  long long int maxn;           //fine all the primes between 2 and maxn
  int printresults;   //print the results or not
  int numprocs;       //the number of processes that are running
  int numworkers;     //the number of workers that are running
  int verbose;
}options;


//prints an integer array
void print_int_arr(int* arr, long long int n){
  long long int i;
  for (i=0;i<n-1;i++){
    printf("%d,",arr[i]);
  }
  printf("%d\n",arr[n-1]);
}

//prints out an options struct
void print_opts(options* o){

  printf("N=2..%lld\n",o->maxn);
  printf("Print Results: %s\n", o->printresults!=0? "Yes":"No");
  printf("Number of processors: %d\n",o->numprocs);
  printf("Number of workers: %d\n",o->numworkers);

}

//Takes the command line arguments and fills up the proper options in the provided options struct. 
//Available options are
//--printresults=<integer>  if the provided option is 0, then no results are printed. Otherwise, results are printed.
//<N>  Check all numbers 2..N for primality.    
void set_opts(int argc, char** argv, options* o){
  int i;
  long long int dummyllint;
  int dummyint;
  o->maxn=1000;
  o->printresults=0;
  MPI_Comm_size(MPI_COMM_WORLD,&dummyint);
  o->numprocs=dummyint;
  o->numworkers=o->numprocs-1;
  o->verbose=0;
  for (i=0;i<argc;i++){
    if(sscanf(argv[i],"--printresults=%d",&dummyint)==1){
      o->printresults=dummyint;
    }
    
    else if (sscanf(argv[i],"%lld",&dummyllint)==1){
      o->maxn=dummyllint;
    }
    else if (strcmp(argv[i],"--verbose")==0){
      o->verbose=1;
    }
    
  }


 
  
}

