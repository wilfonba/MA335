#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<string.h>
#include "arr2str.c"

#define MIN(a,b) ((a)<(b) ? (a): (b))

typedef struct{

  int n;
  int m;
  int nproc;
  char* inputfile;
  double bucket_size_multiplier;
  double a;
  double b;
  int print_data;
  int check_results;
} options;

void print_usage(){
  fprintf(stdout,"bucket sorting options\n");
  fprintf(stdout,"-h or --help                print these options\n");
  fprintf(stdout,"--n= n                      randomly initialize an array of length n for sorting\n");
  fprintf(stdout,"--m= m                      use this many buckets. Defauls to p in parallel, and n/10 in serial\n");
  fprintf(stdout,"--bucket_size_multiplier=n  each bucket will have capacity n/p*bucket_size_multiplier\n");
  fprintf(stdout,"--input_file=filename       read the list for sorting out of filename rather than making it randomly.\n");
  fprintf(stdout,"--range=a-b                random list is evenly distributed between a and b\n");
  fprintf(stdout,"--check-results=<0|1>      if set to 1, check the list at the end to make sure it is sorted.  If set to 0 then don't check. Default to 1.\n");
  fprintf(stdout,"--print_data=<0|1>          print the data?\n");  
}


void print_options(options* opts){

  printf("n: %d\n",opts->n);
  printf("m: %d\n",opts->m);
  printf("nproc: %d\n", opts->nproc);
  printf("input file: %s\n",opts->inputfile);
  printf("data range:[%f-%f]\n",opts->a,opts->b);
  printf("print data: %s\n",opts->print_data ? "YES":"NO");

}

void read_inputfile(char* filename,double** L, options* opts){

  double max=-INFINITY;
  double min=INFINITY;
  int L_capacity=100;
  *L=malloc(L_capacity*sizeof(double));
  int L_len=0;

  FILE* f =fopen(filename,"r");
  if (f==NULL){
    fprintf(stderr,"Error opening %s for reading.\n",filename);
    MPI_Abort(MPI_COMM_WORLD,1);
  }

  double data;
  int numread=0;
  while (!feof(f)){
    numread=fscanf(f,"%lf",&data);
    if (numread>1){
      fprintf(stderr,"Error reading file %s, %d entry\n",filename,L_len);
      MPI_Abort(MPI_COMM_WORLD,1);
    }

    if (numread==1){
      if (L_len>=L_capacity){
	*L = realloc(*L,L_capacity*2*sizeof(double));
	if (*L==NULL){
	  fprintf(stderr,"Error reallocating L\n");
	  MPI_Abort(MPI_COMM_WORLD,1);
	}
	L_capacity*=2;
      
      }
    
      (*L)[L_len]=data;
      min= data < min ? data: min;
      max=data>=max? data+1: max;
      L_len++;
    }
    
  }
    
  opts->n=L_len;
  opts->a=min;
  opts->b=max;

  
}

void read_options(int argc, char** argv, options* opts){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&(opts->nproc));

  int i;
  opts->inputfile=malloc(100*sizeof(char));
  opts->inputfile[0]='\0';
  opts->bucket_size_multiplier=8;

  opts->n=30;
  opts->a=0;
  opts->b=100;

  opts->print_data=2;

  opts->m=-1;
  
  int dummyi;
  double dummyd;
  double dummydd;
  char dummystr[100];
  
  for (i=1;i<argc;i++){

    if (sscanf(argv[i],"--n=%d",&dummyi)==1){
      opts->n=dummyi;
    }
    else if (sscanf(argv[i],"--m=%d",&dummyi)==1){
      opts->m=dummyi;
    }
    else if (sscanf(argv[i],"--bucket_size_multiplier=%lf",&dummyd)==1){
      opts->bucket_size_multiplier=dummyd;
    }
    else if (sscanf(argv[i],"--input_file=%s",dummystr)==1){
      memcpy(opts->inputfile,dummystr,100);
      opts->n=-1;
    }
    else if (strcmp("-h",argv[i])==0 || strcmp("--help",argv[i])==0){
      if (rank==0){
	       print_usage();
      }
      MPI_Abort(MPI_COMM_WORLD,0);
      exit(0);
    }
    else if (sscanf(argv[i],"--range=%lf-%lf",&dummyd,&dummydd)==2){
      opts->a=dummyd;
      opts->b=dummydd;
    }
    else if (sscanf(argv[i],"--print_data=%d",&dummyi)==1){
      opts->print_data=dummyi;
    }
    else if(sscanf(argv[i],"--check-results=%d",&dummyi)==1){
      opts->check_results=dummyi;
    }
    else{
      if (rank==0){
	fprintf(stderr,"Error reading options.\n%s is not recognized.\n",argv[i]);
	print_usage();
      }
      MPI_Abort(MPI_COMM_WORLD,1);
    }
  }

  if (opts->print_data==2){
    if (opts->n>50){
      opts->print_data=0;
    }
    else{
      opts->print_data=1;
    }
  }
  if (opts->m==-1 && opts->nproc==1){
      opts->m=opts->n/10;
  }
  if (opts->nproc!=1){
    opts->m=opts->nproc;
  }
}


//1 if L is sorted ascending, 0 if not sorted ascending.
//n is length of L.
int is_sorted(double* L,int n){

  int i;
  int result=1;
  for (i=1;i<n-1;i++){
    if (L[i]<L[i-1]){
      result=0;
      break;
    }

  }
  return result;

}


void make_random_data(double** L, options* o){
  int i;
  srand(1);
  *L=malloc(o->n*sizeof(double));
  double a= o->a;
  double b=o->b;
  for (i=0;i<o->n;i++){

    (*L)[i]=1.0*rand()/RAND_MAX*(b-a)+a;

  }

}
