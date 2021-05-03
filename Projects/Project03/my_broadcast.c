#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <stdio.h>
#include <math.h>

void my_broadcast(int* buff, int count)
{
    int size;
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int k = 0;
    while(1) {
        if (myRank < pow(2,k)) {
            //printf("Rank %d sending message to %d\n",myRank,myRank*2 + 1);
            MPI_Send(buff,count,MPI_INT,myRank*2 +1,0,MPI_COMM_WORLD);
        }
        printf("I'm here\n");
        if (myRank < pow(2,k + 1) && myRank >= pow(2,k)) {
            //printf("Rank %s receiving messge from %d\n",myRank,myRank/2);
            MPI_Recv(buff,count,MPI_INT,myRank/2,0,MPI_COMM_WORLD,NULL);
        }
        k++;
        printf("Size = %d:    2^k = %d\n",size,pow(2,k));
        if (pow(2,k) >= size) {
            break;
        }
    }
}