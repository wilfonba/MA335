#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

#define DEBUG

int sumArray(int* arr,int count) {
    int i;
    int sum = 0;
    for (i = 0;i < count;i++) {
        sum+=arr[i];
    }
    return(sum);
}

void convBinary(int n,int* rep,int i) {
    if (n > 1) {
        convBinary(n/2,rep,i + 1);
    }
    *(rep + i) = n%2;
}

void my_allplus(int* sendbuf,int* recvbuf, int count)
{
    int size;
    int myRank;
    MPI_Comm_size(MPI_COMM_WORLD,&size);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
    int myLabel = myRank;
    int maxLabel = size - 1;
    int k = 1;
    int offset = 1;
    int* storeBuf = malloc(size*sizeof(int));
    int mySum = sumArray(sendbuf,count);
    storeBuf = memcpy(storeBuf,&mySum,sizeof(int));
    //printf("Rank %d has initial sum %d\n",myRank,*storeBuf);
    int itr = 0;
    // Gather sum at 0
    if (size > 1) {
        while (myLabel%2 == 0 && offset < size) {
            if (myLabel < maxLabel) {
                #ifdef DEBUG
                    //printf("Rank %d receiving from rank %d\n",myRank,(myRank + offset + size)%size);
                #endif
                MPI_Recv(storeBuf,1,MPI_INT,(myRank + offset + size)%size,0,MPI_COMM_WORLD,NULL);
                mySum += *storeBuf; 
            }
            myLabel = myLabel/2;
            offset*=2;
            itr++;
            maxLabel = maxLabel/2;
            *storeBuf = mySum;
            //add numbers here in a not dumb way;
            if (maxLabel == 0) {
                break;
            }
        }
        if (myLabel != 0) {
            #ifdef DEBUG
                printf("Rank %d sending %d to rank %d on itr %d\n",myRank,*(storeBuf + itr),(myRank - offset + size)%size,itr);
                fflush(stdout);
            #endif
            MPI_Send(storeBuf,1,MPI_INT,(myRank - offset + size)%size,0,MPI_COMM_WORLD);
        }
        else {
            //mySum = sumArray(storeBuf,size);
            printf("\n\nThe sum is %d\n\n",*storeBuf);
            *recvbuf = mySum;
        }
        // Send sum to all other nodes
        if (myRank == 0) {
            MPI_Send(recvbuf,1,MPI_INT,1,0,MPI_COMM_WORLD);
        }
        else if (myRank == size - 1) {
            MPI_Recv(recvbuf,1,MPI_INT,size - 2,0,MPI_COMM_WORLD,NULL);
        }
        else if (size > 1) {
            MPI_Recv(recvbuf,1,MPI_INT,myRank - 1,0,MPI_COMM_WORLD,NULL);
            MPI_Send(recvbuf,1,MPI_INT,myRank + 1,0,MPI_COMM_WORLD);
        }
    }
    else {
        printf("My sum is %d\n",mySum);
        *recvbuf = mySum;
    }
    free(storeBuf);
}


int main(int* argc, char** argv) {
    MPI_Init(NULL,NULL);
    int* send = malloc(10*sizeof(int));
    int* recv = malloc(10*sizeof(int));
    int count = 2;
    my_allplus(send,recv,count);
    MPI_Finalize();
}