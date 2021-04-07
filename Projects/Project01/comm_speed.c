#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

typedef struct{
    int size;
    int trials;
} options;

int main(int argc, char** argv)
{
    // Initialize MPI and get numProcs and myRank
    MPI_Init(&argc,&argv);
    int numProcs;
    int myRank;
    MPI_Comm_size(MPI_COMM_WORLD,&numProcs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myRank);

    // Get command line arguments and assign them to options
    options o;
    int dummyInt;
    sscanf(argv[1],"%d",&dummyInt);
    o.size = dummyInt;
    sscanf(argv[2],"%d",&dummyInt);
    o.trials = dummyInt;

    // Check for even number of proccesses
    if (numProcs%2 == 1) {
        printf("Please enter an even number of processors\n");
        return 1;
    }
    
    long long int messageSize = o.size*1024*1024/sizeof(int);
    int* message = calloc(messageSize,sizeof(int));
    int* recvBuff = calloc(messageSize,sizeof(int));
    double* tripSpeed = malloc(o.trials*sizeof(double));
    MPI_Status status;
    int i = 0;
    for (i;i < o.trials;i++) {
        double startTime = MPI_Wtime();
        if (myRank%2 == 0) {
            MPI_Send(message,messageSize,MPI_INT,myRank + 1,0,MPI_COMM_WORLD);
            MPI_Recv(message,messageSize,MPI_INT,myRank + 1,0,MPI_COMM_WORLD,&status);
            double endTime = MPI_Wtime();
            tripSpeed[i] = o.size/((endTime - startTime)/2); 
        }
        else {
            MPI_Recv(recvBuff,messageSize,MPI_INT,myRank - 1,0,MPI_COMM_WORLD,&status);
            MPI_Send(recvBuff,messageSize,MPI_INT,myRank -1,0,MPI_COMM_WORLD);
        }   
    }

    double avgSpeed = 0;
    for (i = 0;i < o.trials; i++)
    {
        avgSpeed += tripSpeed[i];
    }
    avgSpeed = avgSpeed/o.trials;

    if (myRank%2 == 0)
        printf("%d <--> %d Transmission Rate: %f MB/S\n",myRank,myRank + 1,avgSpeed);
    MPI_Finalize();
}
