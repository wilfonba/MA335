#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#include<string.h>

int __real_MPI_Finalize();
int __real_incircle(double,double);
void __real_srand(unsigned int);

extern int times_palindrome_checked;

int __wrap_MPI_Finalize(){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  char filename[20];
  sprintf(filename,"whatidid.rank.%d",rank);
  FILE* f = fopen(filename,"w");
  int i;
  fprintf(f,"palindrome calls: %d\n",times_palindrome_checked);
  fclose(f);
  return __real_MPI_Finalize();

}
