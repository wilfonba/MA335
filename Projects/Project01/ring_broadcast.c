#include<stdlib.h>
#include<stdio.h>
#include<mpi.h>

void getInput(int* N)
{
    char input[100];
    printf("Please enter an integer:");
    fflush(stdout);
    fgets(input,100,stdin);
    sscanf(input,"%d",N);
}

int main(int argc, char** argv)
{
    MPI_Init(&argc,&argv);
    int numProcs;
    int myRank;
    MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    int N = 1;
    int numb = 1;
    int temp;
    MPI_Status status;
    while (N >= 0 && numb >= 0)
    {
        if (myRank == 0)
        {
            getInput(&N);
            int temp;
            MPI_Status status;
            MPI_Send(&N,1,MPI_INT,1,0,MPI_COMM_WORLD);
            fflush(stdout);
        }
        else if (myRank <= numProcs - 2)
        {
            MPI_Recv(&numb,1,MPI_INT,myRank-1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            MPI_Send(&numb,1,MPI_INT,myRank + 1,0,MPI_COMM_WORLD);
            printf("My rank is %2d and I received the interger %d\n",myRank,numb);
        }
        else
        {
            MPI_Recv(&numb,1,MPI_INT,myRank-1,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
            printf("My rank is %2d and I received the interger %d\n",myRank,numb);

        }
    }
    MPI_Finalize();
    return 0;
}