#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "arr2str.c"

int main(int argc, char** argv) {
  MPI_Init(&argc,&argv);
  int total_procs;
  int rank;
  MPI_Comm_size(MPI_COMM_WORLD,&total_procs);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  int i;
 
  //What is the "root" of the scatter?  The root is the process that sends
  //all the data out. 
  int root=0;

  //How long will a messge be?
  int msg_len=5;

  //specify the root of the scatter, default to 0
  if (argc>1) {
    sscanf(argv[1],"%d",&root);
  }
  //Read message length if supplied.
  if (argc>2){
    sscanf(argv[2],"%d",&msg_len);
  }

  //Print out the options.
  if (rank==0){
    printf("Demonstrating MPI_Scatter.\n");
    printf("Root of Scatter: %d\n",root);
    printf("Message Length: %d\n\n",msg_len);
    fflush(stdout);
  }

  MPI_Barrier(MPI_COMM_WORLD);

  //If you are the root, make up messages for each processor, put them in one buffer,
  //and call the scatter. 
  if (rank==root){
    //Make up a random message for each processor.
    int* M_array=malloc(msg_len*total_procs*sizeof(int));
    //Seed random number generator. 
    srand(time(NULL)+rank*10000);
    //Fill in your message.
    for (i=0;i<msg_len*total_procs;i++){
      M_array[i]=rand() % 10;
    }    

    printf("Rank %d has messages: %s\n",rank,int_arr2str(M_array,msg_len*total_procs));

    //Have a place to recieve your message, yes, even the root recieves the message. 
    int* M=malloc(msg_len*sizeof(int));

    //Do the scatter. This says "Do a scatter, send the data in M_array, send msg_len of them to
    //each processor, they are of type MPI_INT.  Put my message in M, make sure it is no longer 
    //than msg_len, and is of type MPI_INT.  The root of this scatter is root, on MPI_COMM_WORLD".
    MPI_Scatter(M_array,msg_len,MPI_INT,M,msg_len,MPI_INT,root,MPI_COMM_WORLD);

    //Print out the message you have. 
    printf("Rank: %d M:%s\n",rank,int_arr2str(M,msg_len));
  } else {
    //If you aren't the root, then you don't need a send buffer. Just make a recive buffer, and 
    //get your message. 
    int* M=malloc(msg_len*sizeof(int));

    MPI_Scatter(NULL,0,MPI_INT,M,msg_len,MPI_INT,root,MPI_COMM_WORLD);

    //Print out what you got. 
    printf("Rank: %d M:%s\n",rank,int_arr2str(M,msg_len));
  }



  MPI_Finalize();

}
