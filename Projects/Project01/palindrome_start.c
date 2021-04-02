#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<mpi.h>
#include<math.h>

#include "dict.c"


typedef struct{
  int numwords;
  char* dictfile;
}options;

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



  
  MPI_Finalize();
  exit(MPI_SUCCESS);
}
