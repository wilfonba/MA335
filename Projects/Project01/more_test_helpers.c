#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<string.h>

int __real_MPI_Finalize();
void __real_srand(unsigned int);
int __real_MPI_Init(int*, char***);

int srand_times=0;
int* srand_seeds=NULL;
int logsize=10;
int srand_base_seed;

extern int crosses_line_calls;


void __wrap_srand(unsigned int seed){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  
  if (srand_base_seed!=-1000){
    seed=srand_base_seed+rank;
  }
  
  if (srand_seeds==NULL){
    srand_seeds=malloc(logsize*sizeof(unsigned int));
  }
  if (srand_times<logsize){
    srand_seeds[srand_times]=seed;
  }
  srand_times++;


  __real_srand(seed);
}

int __wrap_MPI_Finalize(){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  char filename[20];
  sprintf(filename,"whatidid.rank.%d",rank);
  FILE* f = fopen(filename,"w");
  int i;
  fprintf(f,"Times srand seeded: %d\n",srand_times);
  fprintf(f,"seeds used in srand: ");
  for (i=0;i<srand_times-1;i++){
    fprintf(f,"%d,",srand_seeds[i]);
  }
  if (srand_times>0){
    fprintf(f,"%d\n",srand_seeds[srand_times-1]);
  }
  fprintf(f,"crosses_line_calls: %d\n",crosses_line_calls);
  fclose(f);
  return __real_MPI_Finalize();

}

int __wrap_MPI_Init(int* argc,char*** argv){

  int status;
  status=__real_MPI_Init(argc,argv);
  int i;
  char** real_argv=malloc(*argc*sizeof(char*));
  int real_argc=0;
  int dummyi;
  srand_base_seed=-1000;
  for (i=0;i<*argc;i++){
    if (sscanf((*argv)[i],"--base-seed=%d",&dummyi)==1){
      srand_base_seed=dummyi;
    }
    else{
      real_argv[real_argc]=malloc(strlen((*argv)[i])*sizeof(char));
      strcpy(real_argv[real_argc],(*argv)[i]);
      real_argc++;
    }
  }

  *argc=real_argc;
  *argv=real_argv;
  
  return status;
}
