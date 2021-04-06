#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "helpers_dynamic.h"

//Return 0 if n is composite, 
//Return 1 if n is prime. 
int is_prime(int n){

  int i;
  if (n<=1) {
  	return 0;
  }
  for (i=2;i<n;i++) {
    if (n%i==0) {
      return 0;
    }
  }

  return 1;
}

//Find the minimum of integers a and b. I'm sure this exists in math.h 
//but I don't feel like messing with it. 
int min(int a,int b) {
	if (a<b) {
		return a;
	} else {
		return b;
	}
}

//A list of things you want debugging output for. 
int debug_list_len=2;
char* debug_list[100]={"send_task","recv_message"};

//Returns 0 if name is in the debug_list, 0 otherwise. 
int do_debug(char* name) {
	int i;
	int name_in_list=0;
	for (i=0;i<debug_list_len;i++) {
		if (strcmp(name,debug_list[i])==0) {
			name_in_list=1;
			break;
		}
	}
	return name_in_list;
}



/* In this code, a unit of work, called a task,  is a range of integers (inclusive) to check for primality.
   Each task has an ID associated with it.  So a task is represented with three
   numbers: (TaskID, lower_bound, upper_bound).  For example, the task (34, 179,
   356)  is task number 34, and means "check 179, 180, 181, ..., 356" for
   primality. 

   The results of a unit of work, called a result, consists of an ID matching
   the task from whence the results come, an integer identifying how many
   integers are prime, and then the list of primes found.  So, for this task:
   (2,10,20) the proper result is (2,4,11,13,17,19).  The 2 means "these are the
   results for task 2" The 4 means "in the range in this task, I found 4 prime
   numbers" The 11,13,17,19 is the list of prime numbers found for task 2. */



/* Here is a key for what the message tags mean.  We could C ENUMs, but we don't
   need MORE language features in play here.

   TAG  -  USE
    0   -  Sending a task to a worker node.
    1   -  Sending the first part (ID and length) from a worker to the root.
    2   -  Sending the last part (list of primes) for a result from a worker to
           the root. 
    3   -  Root sending a quit message to a worker.*/
#define TAG_TASK 0
#define TAG_RESULT_INIT 1
#define TAG_RESULT_DATA 2
#define TAG_QUIT 3

//Function to the rank on the world communicator.  Pure convenience.  Possibly
//could be replaced by a global variable. 
int my_rank() {
	int rank;
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	return rank;
}


//Send the following task to rank: (taskID,min,max)
void send_task(int rank, int taskID,int min, int max) {
	//arrange the data into a single array. 
	int tmp[3]={taskID,min,max};
	//Call MPI_Send, with the proper tag
	if (do_debug("send_task")){
		printf("Root sending task (%d,%d,%d) --> %d\n",tmp[0],tmp[1],tmp[2],rank);
	}
	MPI_Send(tmp,TAG_QUIT,MPI_INT,rank,0,MPI_COMM_WORLD);
}

//Recieve a message from the root.  This message will either be a task
//assignment, or it will be the root telling the worker to quit.  If it is a
//task, set quit=0 and populate taskID, min, and max.  Otherwise, set quit=1. 
void recv_message(int* taskID, int* min,int* max,int* quit){
	//Make a buffer to store the task -- task messages are at most 3 ints long. 
	int tmp[3];
  //Recieve a message of length at most three from rank 0. Let the tag be
  //anything, and then figure out what the tag was later. 
	MPI_Status status;	 

	if (do_debug("recv_message")){
		printf("Rank %d waiting for a message from the root\n",my_rank());
	}

	MPI_Recv(tmp,TAG_QUIT,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
	//See if the tag on the message you recieved was 3 (quit) or 0 (task)
  if (status.MPI_TAG==TAG_TASK) {  
  //Got a task, so copy data out of buffer to the proper variables. 
		*quit=0;
		*taskID=tmp[0];
		*min=tmp[1];
		*max=tmp[2];
	} else if(status.MPI_TAG==TAG_QUIT) { //Got a quit message. 
		*quit=1;
	} else { 
    //Don't know what we got!  Print an error message and kill all processes. 
		fprintf(stderr,"Error! Rank %d recieved message with unexpected tag %d from root\n",my_rank(),status.MPI_TAG);
    //Kills all processes on world communicator -- they all return error code 1.
		MPI_Abort(MPI_COMM_WORLD,1); 
	}

	if (do_debug("recv_message")) {
		if (*quit==1) {
			printf("Rank %d recieved a quit message.\n",my_rank());
		} else {
			printf("Rank %d recieved task (%d,%d,%d).\n",my_rank(),*taskID,*min,*max);
		}
	}
}

//Send a result back to the root.  This must be done in two stages, to allow the
//root to allocate a properly sized buffer for the results. 
void send_results(int taskID, int* prime_list,int num_primes){

	if (do_debug("send_results")) {
		printf("Rank %d sending results for task %d back.\n",my_rank(),taskID);
	}

  //Tell the root what taskID this is for, and how many primes you'll be sending
  //back.  Use tag 1
	int tmp[2]={taskID,num_primes};
	MPI_Send(tmp,2,MPI_INT,0,TAG_RESULT_INIT,MPI_COMM_WORLD);
  //Now, the root will be waiting for results from me.  Send back the actual
  //results, with tag 2.
 	MPI_Send(prime_list,num_primes,MPI_INT,0,TAG_RESULT_DATA,MPI_COMM_WORLD);

 	if (do_debug("send_results")) {
 		printf("Rank %d done sending back results for task %d.\n",my_rank(),taskID);
 	}
}

//Recieve results back from a worker. 
//The results are stored in a 2D array:
//primes_list[taskID]={list of primes out of task taskID}
//we don't know ahead of time how many prime numbers there are in task taskID, 
//So we have to allocate that as we go along. 
//For use later, we'll need to keep track of how long each of those arrays are,
//so we'll have
//num_primes[taskID]=how many primes there are in primes_list[taskID]
//Since primes_list[taskID] is an array, aka, an int*, 
//primes_list must be an int**.
//Similarly, num_primes is an int*
//In addition to getting and storing results, recv_results returns what worker
//send the results, so that we can assign another task or tell it to quit. 
void recv_results(int** primes_list, int* num_primes,int* workerID){
  //First, wait for a message from a worker with tag 1, the start of a results
  //transmission. 
  //The start of a results transmission is 2 ints long.  You want to get results
  //from any worker
	//that is ready. 
	int buf[2];
	MPI_Status stat;
	MPI_Recv(buf,2,MPI_INT,MPI_ANY_SOURCE,TAG_RESULT_INIT,MPI_COMM_WORLD,&stat);
	//Break the buffer into the parts you want. 
	int taskID=buf[0];
	int len=buf[1];

  //Figure out who the message came from, both so that you can be sure to get
  //the second half of the result from the same worker, and so that you can then
  //assign some more work to that worker. 
	*workerID=stat.MPI_SOURCE;
	if (do_debug("recv_results")){
		printf("Root recieved initial message for result %d, from worker %d, it has %d primes in it.\n",taskID,*workerID,len);
	}

  //Ok, now allocate enough room in primes_list[taskID], and set
  //num_primes[taskID].
	primes_list[taskID]=malloc(len*sizeof(int));
	num_primes[taskID]=len;

  //Actually to the recieving now -- note: you ONLY want to recieve the second
  //half of the message from worker.  You don't want to accidentally get the
  //start of someone else's results!
  //You can pretty well ignore the status of this message -- you know who it
  //came from and how long it was.
  MPI_Recv(primes_list[taskID], num_primes[taskID], MPI_INT, *workerID, 
           TAG_RESULT_DATA, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	if (do_debug("recv_results")){
		printf("Root finished recieving results for task %d.\n",taskID);
	}
}

//Send a quit message to rank.  Those are identified by tag 3. 
//It is ok to send a message of length 0. 
void send_quit(int rank){
	if (do_debug("send_quit")){
		printf("Root telling rank %d to quit.\n",rank);
	}
	MPI_Send(NULL,0,MPI_INT,rank,TAG_QUIT,MPI_COMM_WORLD);
}


//This is what a worker does. Pretty easy, just recieve tasks, perform the task,
//send the results back, until the root tells you to quit.  One interesting
//thing is that it is not known ahead of time how many primes will be found for
//a given task, and therefore not clear how much memory to allocate.  We could
//certainly just allocate "enough", but let's just grow the array as we need to! 
void workerstuff(options* o){
	//Who am i?
	int rank = my_rank();

	//Keep track of total time I spend working:
	double start=MPI_Wtime();

	//Keep track of how many integers I check overall;
	int total_ints_checked=0;

	//These variables define a task to complete. 
	int taskID;
	int min;
	int max;

	//Each time we work on a task, we need to store the primes we find, 
	//and we need to keep track of how many there are. 
	//We will initially allocate enough room to store 5 primes (waayyyyy too few.)
	//in primes_list, and we will grow primes_lest as need be. 
	int num_primes_found;
	int initial_primes_list_capacity=5;
	int primes_list_capacity;
	int* primes_list;

	int i;
	int quit;
	//Keep doing this loop until told to stop!
	while (1) {
		//Get an assignment from the root. 
		recv_message(&taskID,&min,&max,&quit);
		//If it was a quit message, then quit. 
		if (quit){
			break;
		}
		//allocate some room for the list of primes. 
		primes_list=malloc(initial_primes_list_capacity*sizeof(int));
		//keep track of it's current capacity.
		primes_list_capacity=initial_primes_list_capacity;

		//num_primes_found is how many things you have put in primes_list so far. 
		num_primes_found=0;
		//Ok, go looking for primes!
		for (i=min;i<=max;i++){
		  	total_ints_checked++;  //for record keeping
			if (is_prime(i)){
        //Ok, great, i is prime.  You need to append it to the end of
        //primes_list.  BUT, you need to make sure that you have room to do so
        //first.  If you don't, then use realloc to make a new buffer, and copy
        //the contents of primes_list to the new buffer. 
				if (num_primes_found==primes_list_capacity){
					//make a new block of memory with size 2*primes capacity, and copy
					//the contents of prime_list to the first half of it. 
					primes_list=realloc(primes_list,2*primes_list_capacity*sizeof(int));
					//Now, the capacity of primes_list has grown, so update it. 
					primes_list_capacity*=2;
				}
				//stick i at the end of primes_list.
				primes_list[num_primes_found]=i;
				num_primes_found++;
			}
		}

		if (do_debug("worker_loop")){
			printf("Rank %d found primes:",rank);
			print_int_arr(primes_list,num_primes_found);
			printf("in the range [%d,%d]\n",min,max);
		}
		//send your results back to the root. 
		send_results(taskID,primes_list,num_primes_found);

		//You are allocating space in a loop! Be sure to free it in a loop!
		free(primes_list);
	}

	//Stop the timer so you know how long you worked. 
	double end=MPI_Wtime();
	if (do_debug("timing") || do_debug("worker") || o->print_timing){
    printf("Rank %d exiting after checking %d ints and working %.3lf seconds\n",
           rank,total_ints_checked,end-start);
	}
}

//This is what the master should do.  At its core, pretty simple.  Send out
//tasks to workers until they are all completed. 
void masterstuff(options* o) {

	double start=MPI_Wtime();  //keep track of how long we are working. 

	//First, figure out how many tasks there will be.
	//The length of a range assigned in a task is controlled by 
	//o->taskchunksize. 

	int num_tasks=(o->maxn-1) / o->taskchunksize;
	//If not evenly divisible into tasks, there will be one additional task. 
	if ((o->maxn-1) % o->taskchunksize != 0) {
		num_tasks++;
	}

	//Need a spot to hold our results! 
	//primes_list[taskID] will hold an array of the primes found for taskID, so
	//primes_list[0] = {2,3,5, ... } an array of primes in the first task. 
	//that means that primes_list[i] is an int*, so primes_list is an int**
	int** primes_list=malloc(num_tasks*sizeof(int*));
	//Keep track of how long each results list is. 
  //num_primes[taskID] should give the number of primes stored in
  //primes_list[taskID]
	int* num_primes=malloc(num_tasks*sizeof(int));

	//Keep track of how many tasks have been assigned. 
	int num_tasks_assigned=0;
	//Keep track of how many tasks have been completed. 
	int num_tasks_completed=0;

	//keep track of the last task assigned. 
	int last_task[3]={-1,1,1};

  //First, assign a task to all workers, unless there are no tasks left to be
  //assigned. 
	int i;
	for (i=1;i<o->numprocs;i++) {
		if (num_tasks_assigned<num_tasks) {
			last_task[0]++;              //increment the taskID
			last_task[1]=last_task[2]+1; //start this task at one past the last task. 

      //this task goes to either the taskchunksize, or  the end of what we want
      //to check.
			last_task[2]=min(last_task[1]+o->taskchunksize-1,o->maxn);
			send_task(i,last_task[0],last_task[1],last_task[2]); 
			num_tasks_assigned++;
		} else {
       //if there is no task left to assign, tell the worker to quit. 
			send_quit(i);
		}
	}

	int workerID;
  //Ok, now just keep recieving results and assigning new tasks until all tasks
  //are completed.
	while (num_tasks_completed<num_tasks){
    //Recv results from anyone, put the results in primes_list, and update the
    //num_primes array.  Figure out who send you results so that you can assign
    //them more work. 
		recv_results(primes_list,num_primes,&workerID);
		num_tasks_completed++;
		//If there is anything left to do, tell workerID to do some. 
		if (num_tasks_assigned<num_tasks){
			last_task[0]++;              
			last_task[1]=last_task[2]+1; 
			last_task[2]=min(last_task[1]+o->taskchunksize-1,o->maxn);
			send_task(workerID,last_task[0],last_task[1],last_task[2]); 
			num_tasks_assigned++;
		} else {
			send_quit(workerID);
		}

	}

	double end=MPI_Wtime();

	if (o->printresults) {
		printf("\n\nPrimes Found:\n");
		int total=0;
		for (i=0;i<num_tasks;i++) {
			total+=num_primes[i];
			printf("(Task %d) ",i);
			print_int_arr(primes_list[i],num_primes[i]);
		}
		printf("%d primes found in total.\n\n",total);
	}

	if (do_debug("master") || do_debug("timing")||o->print_timing){
		printf("Master exiting after working  %.3lf seconds.\n",end-start);
	}
}


int main(int argc, char** argv){
  MPI_Init(&argc,&argv);
  int rank;
  int numproc;

  MPI_Comm_size(MPI_COMM_WORLD,&numproc);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  
  //Let's quit if there is only one processor.
  if (numproc==1) {
  	printf("Please run this code with at least two processors.\n");
  	MPI_Finalize();
  	return(0);
  }


  options o;
  set_opts(argc,argv,&o);
  if (rank==0) {
    print_opts(&o);
    masterstuff(&o);
  } else {
    workerstuff(&o);
  }
  
  

  MPI_Finalize();
  return 0;
}
