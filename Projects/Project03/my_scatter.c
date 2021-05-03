#include <stdlib.h>
#include <mpi.h>
#include <string.h>

void my_scatter(int* sendbuf,int sendcount,int* recvbuf,int recvcount,int root)
{
    int myRank;
    int size;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);

    if (myRank != root) {
        MPI_Recv(recvbuf,recvcount,MPI_INT,root,0,MPI_COMM_WORLD,NULL);                                                             
    }   
    else {
        int i = 0;
        int* messageBuff = malloc(sendcount*sizeof(int));
        for (i = 0;i < size;i++) {
            if (i != root) {
                messageBuff = memcpy(messageBuff,sendbuf + (i)*sendcount,sendcount*sizeof(int));
                MPI_Send(messageBuff,sendcount,MPI_INT,i,0,MPI_COMM_WORLD);
            }
            else {
                recvbuf = memmove(recvbuf,sendbuf + (i)*sendcount,sendcount*sizeof(int));
            }
        }
        free(messageBuff);
    }
}