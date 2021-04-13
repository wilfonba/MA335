#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<mpi.h>
#include<math.h>

#include "dict.c"

#define debug

typedef struct{
  int numwords;
  char* dictfile;
}options;

typedef struct{
  int lowerBound;
  int upperBound;
}bounds;

void print_options(options* opt) {
  printf("numwords: %d\n",opt->numwords);
  printf("dictfile: %s\n",opt->dictfile);
}

void print_usage() {
  printf("A palindrome finder.\nUsage:\n");
  printf("mpirun palindrome <options>\n");
  printf("Valid options are:\n");
  printf("--numwords=<k>   means look for palindromes consisting of k words. Defaults to 2.\n");
  printf("--dictfile=<filename>    means use the dictionary stored in dictfile.  Defautls to dict.txt\n");
}

void parse_options(int argc, char** argv, options* opt) {
  int i;
  char dummys[1000];
  int dummyi;
  opt->numwords=2;
  opt->dictfile=malloc(1000*sizeof(char));
  strcpy(opt->dictfile,"dict.txt");
  for (i=1;i<argc;i++) {
    if (sscanf(argv[i],"--dictfile=%s",dummys)==1) {
      strcpy(opt->dictfile,dummys);
    } else if (sscanf(argv[i],"--numwords=%d",&dummyi)==1) {
      opt->numwords=dummyi;
    } else {
      fprintf(stderr,"Error, %s is not a recognized option.\n", argv[i]);
      print_usage();
      exit(1);
    }
  }

}

long long int times_palindrome_checked=0;

int ispalindrome(char* word) {
  times_palindrome_checked++;
  int i;
  int l=strlen(word);
  int answer=1;

  for (i=0;i<l/2;i++) {
    if (word[i]!=word[l-1-i]) {
      answer=0;
      break;
    }
  }
  return answer;
    
}

int* checkMyWords(int LB,int UB,long long int N,dictionary* D,options* opt) {
  printf("I'm allocating memory\n");
  double start = MPI_Wtime();
  int** toCheck = malloc(N*sizeof(int*));
  int i;
  for (i = 0;i < N;i++)
  {
    toCheck[i] = malloc(opt->numwords*sizeof(int));
  }
  double end = MPI_Wtime();
  printf("It took %lf seconds to allocate memory\n",end-start);
}

void root_stuff(dictionary* D,int* size,options* opt) {
  // allocate arrays for assignment steps
  bounds* procBounds = malloc((*size)*sizeof(bounds));
  int* procN = malloc((*size)*sizeof(int));

  // Assign "how many words each process will check"
  int i;
  for (i = 0;i < *size;i++) {
    procN[i] = D->size/(*size);
    if (i < (D->size % *size)) {
      procN[i]++;
    }
  }
  // Assign upper and lower bounds to send to each processor
  procBounds[0].lowerBound = 0;
  procBounds[0].upperBound = procN[0] - 1;
  for (i = 1;i < *size;i++) {
    procBounds[i].lowerBound = procBounds[i-1].upperBound + 1;
    procBounds[i].upperBound = procBounds[i].lowerBound + procN[i] - 1;
  }

  #ifdef debug
    printf("The dictionary has %d words \n",D->size);
    for (i = 0;i < *size;i++) {
      printf("Process %d checking words [%d, %d] for a total of %d words \n",i,procBounds[i].lowerBound,procBounds[i].upperBound,
        procBounds[i].upperBound - procBounds[i].lowerBound + 1);
    }
  #endif

  // Get the upper and lower bounds for the root process
  int myLB = procBounds[0].lowerBound;
  int myUB = procBounds[0].upperBound;
  
  #ifdef debug
    printf("I am process 0 checking the range [%d, %d]\n",myLB,myUB);
  #endif

  // Send the ranges to each worker process
  int buffer[2];
  for (i = 1;i < *size;i++) {
    buffer[0] = procBounds[i].lowerBound;
    buffer[1] = procBounds[i].upperBound;
    MPI_Send(buffer,2,MPI_INT,i,0,MPI_COMM_WORLD);
  }
}

void worker_stuff(dictionary* D,int* rank,options* opt) {
  int buffer[2];
  MPI_Status status;
  MPI_Recv(buffer,2,MPI_INT,0,0,MPI_COMM_WORLD,&status);
  int myLB = buffer[0];
  int myUB = buffer[1];
  int myNumWords = buffer[1] - buffer[0] + 1;
  long long int myN = myNumWords*pow(D->size,(opt->numwords-1));

  #ifdef debug
    printf("I am process %d checking the range [%d, %d]\n",*rank,myLB,myUB);
  #endif

  checkMyWords(myLB,myUB,myN,D,opt);  

}
  
void main(int argc, char** argv) {
  MPI_Init(&argc,&argv);
  
  options opt;
  parse_options(argc,argv,&opt);
  int rank;
  int size;
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  
  if (rank==0) {
    print_options(&opt);
  }

  dictionary D;
  dict_open(opt.dictfile,&D);
  //Now, D.data[i] is a string with the i'th word in the dictionary in it. 
  
  if (rank == 0) {
    root_stuff(&D,&size,&opt);
  }
  else {
    worker_stuff(&D,&rank,&opt);
  }

  MPI_Finalize();
  exit(MPI_SUCCESS);
}
