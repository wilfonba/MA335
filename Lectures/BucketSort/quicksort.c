#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
#include<math.h>
#include<string.h>

#include"helpers.h"


int main(int argc, char** argv){
  MPI_Init(&argc,&argv);
  options o;
  read_options(argc,argv,&o);

  print_options(&o);

  double* L;
  
  if (o.n==-1){
    read_inputfile(o.inputfile,&L,&o);
  }
  else{
    make_random_data(&L,&o);
  }

  if (o.print_data){
    printf("L: ");
    print_arr(L,o.n);
  }
  
  double start=MPI_Wtime();
  qsort(L,o.n,sizeof(double),compare);
  double end=MPI_Wtime();

  if (o.print_data){
    printf("L: ");
    print_arr(L,o.n);
  }
  printf("Elapsed time: %f seconds\n",end-start);
  MPI_Finalize();
  return(0);
}




