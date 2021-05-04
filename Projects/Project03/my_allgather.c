#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include <math.h>


void my_allgather(int* sendbuf, int sendcount, int * recvbuf)
{
    int size;
    int myRank;
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    int i = 0;
    int n;
    memcpy(recvbuf + myRank*sendcount,sendbuf,sendcount*sizeof(int));
    for (i = 0;i < size - 1;i++) {
        n = i*sendcount;
        MPI_Status status;
        int* temp = malloc(sendcount*sizeof(int));
        if (myRank == 0) {
            MPI_Recv(temp,sendcount,MPI_INT,size - 1,0,MPI_COMM_WORLD,&status);
            MPI_Send(sendbuf,sendcount,MPI_INT,1,0,MPI_COMM_WORLD);
        }
        else if (myRank < size - 1 && myRank > 0 && myRank%2 == 0) {
            MPI_Send(sendbuf,sendcount,MPI_INT,myRank + 1,0,MPI_COMM_WORLD);
            MPI_Recv(temp,sendcount,MPI_INT,myRank - 1,0, MPI_COMM_WORLD,&status);
        }
        else if (myRank < size - 1 && myRank > 0 && myRank%2 != 0) {
            MPI_Recv(temp,sendcount,MPI_INT,myRank - 1,0, MPI_COMM_WORLD,&status);
            MPI_Send(sendbuf,sendcount,MPI_INT,myRank + 1,0,MPI_COMM_WORLD);
        }
        else {
            MPI_Send(sendbuf,sendcount,MPI_INT,0,0,MPI_COMM_WORLD);
            MPI_Recv(temp,sendcount,MPI_INT,myRank - 1,0,MPI_COMM_WORLD,&status);
        }
        //MPI_Barrier(MPI_COMM_WORLD);
        int k = (int)fmod(myRank - i + 1,size)*sendcount;
        memcpy(recvbuf + k,temp,sendcount*sizeof(int));
        memcpy(sendbuf,temp,sendcount*sizeof(int));
    }
}