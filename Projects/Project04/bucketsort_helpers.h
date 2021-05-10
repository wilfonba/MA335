#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<string.h>

#define MIN(a,b) ((a)<(b) ? (a): (b))



typedef struct{

  int n;
  int nproc;
  char* inputfile;
  char* outputfile;
  double bucket_size_multiplier;
  double a;
  double b;
  int check_results;
  int print_things;
  int produce_outputfile;
} options;


//print an array
void print_arr(double* L, int n){

  int i;
  printf("[");
  for (i=0;i<n-1;i++){
    printf("%f ",L[i]);
  }
  if (n>0){
    printf("%f",L[n-1]);
  }
  printf("]\n");
}


//print an integer array
void print_int_arr(int* L, int n){

  int i;
  printf("[");
  for (i=0;i<n-1;i++){
    printf("%d ",L[i]);
  }
  if (n>0){
    printf("%d",L[n-1]);
  }
  printf("]\n");
}




void print_usage(){
  fprintf(stdout,"bucket sorting options\n");
  fprintf(stdout,"-h or --help                print these options\n");
  fprintf(stdout,"--n= n                      randomly initialize an array of length n for sorting\n");
  fprintf(stdout,"--bucket_size_multiplier=n  each bucket will have capacity n/p*bucket_size_multiplier\n");
  fprintf(stdout,"--input_file=filename       read the list for sorting out of filename rather than making it randomly.\n");
  fprintf(stdout,"--output_file=filename     the name of the output file to produce.  Defaults to sorted.txt");
  fprintf(stdout,"--produce_outputfile=<0|1> if set to 0, do not produce a sorted list via file.  Defaults to 1\n");
  fprintf(stdout,"--range=a-b                random list is evenly distributed between a and b\n");
  fprintf(stdout,"--check-results=<0|1>      if set to 1, check the list at the end to make sure it is sorted.  If set to 0 then don't check. Default to 1.\n");
  fprintf(stdout,"--print=<0|1>              if set to 1, then will print the input array and the output array. Defaults to 1 if length of input array is <= 50, and defaults to 0 if length on input array is >50.\n");
}


void print_options(options* opts){

  printf("n: %d\n",opts->n);
  printf("nproc: %d\n", opts->nproc);
  printf("input file: %s\n",opts->inputfile);
  printf("output file: %s\n",opts->outputfile);
  printf("Produce outputfile: %s\n",opts->produce_outputfile==1?"YES":"NO");
  printf("data range:[%f-%f]\n",opts->a,opts->b);
  printf("print: %s\n",opts->print_things==1?"YES":"NO");

}

void  read_inputfile(char* filename,double** L, options* opts){

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

void parse_options(int argc, char** argv, options* opts){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  MPI_Comm_size(MPI_COMM_WORLD,&(opts->nproc));

  int i;
  opts->inputfile=malloc(100*sizeof(char));
  opts->inputfile[0]='\0';
  opts->outputfile=malloc(1000*sizeof(char));
  opts->outputfile[0]='\0';
  strcpy(opts->outputfile,"sorted.txt");
  opts->bucket_size_multiplier=8;
  opts->produce_outputfile=1;
  opts->n=30;
  opts->a=0;
  opts->b=100;
  opts->print_things=2;
  int dummyi;
  double dummyd;
  double dummydd;
  char dummystr[100];
  
  for (i=1;i<argc;i++){

    if (sscanf(argv[i],"--n=%d",&dummyi)==1){
      opts->n=dummyi;
    }
    else if (sscanf(argv[i],"--bucket_size_multiplier=%lf",&dummyd)==1){
      opts->bucket_size_multiplier=dummyd;
    }
    else if (sscanf(argv[i],"--input_file=%s",dummystr)==1){
      strcpy(opts->inputfile,dummystr);
      opts->n=-1;
    }
    else if (sscanf(argv[i],"--output_file=%s",dummystr)==1){
      strcpy(opts->outputfile,dummystr);
    }
    else if (strcmp("-h",argv[i])==0 || strcmp("--help",argv[i])==0){
      if (rank==0){
	print_usage();
      }
      MPI_Finalize();
      exit(0);
    }
    else if (sscanf(argv[i],"--range=%lf-%lf",&dummyd,&dummydd)==2){
      opts->a=dummyd;
      opts->b=dummydd;
    }
    else if(sscanf(argv[i],"--check-results=%d",&dummyi)==1){
      opts->check_results=dummyi;
    }
    else if (sscanf(argv[i],"--print=%d",&dummyi)==1){
      opts->print_things=dummyi;
    }
    else if (sscanf(argv[i],"--produce_outputfile=%d",&dummyi)==1){
      opts->produce_outputfile=dummyi;
    }
    else{
      if (rank==0){
	fprintf(stderr,"Error reading options.\n%s is not recognized.\n",argv[i]);
	print_usage();
      }
      MPI_Abort(MPI_COMM_WORLD,1);
    }
  }

  if (opts->print_things==2){
    if (opts->n<=50){
      opts->print_things=1;
    }
    else{
      opts->print_things=0;
    }
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


void write_outputfile(char* filename,double* L, int len){
  FILE* f=fopen(filename,"w");
  if (f==NULL){
    fprintf(stderr,"Error opening %s for writing.\n",filename);
    MPI_Abort(MPI_COMM_WORLD,1);
  }
  int i;
  for (i=0;i<len;i++){
    fprintf(f,"%lf\n",L[i]);
  }
  fclose(f);



}


