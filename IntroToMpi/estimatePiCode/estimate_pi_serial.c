#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>


//This is a program that estimates pi by throwing random darts at a 1x1
//dartboard, and recording how many fall within 1 quadrant of the unit circle. 


//To use the program, do 
//./estimate_pi_serial <number of darts to throw> 
//if <number of darts to throw is not given on the command line, the default is 1000. 
int main(int argc, char** argv){

  int N;
  //test to see if the user gave us an extra command line argument for number of trials. 
  if (argc<2){
    N=1e3;
  }
  else{
    N=atoi(argv[1]);
  }

  //seed the random number generator 
  //any time you want random numbers you must seed the random number generator. 
  int basetime=time(NULL);
  srand(basetime);

  //Throw a dart at the square [0,1] x [0,1] numtrials times.  Record how many times
  //the dart lands inside the unit circle. 
  double x,y;
  int successes=0;
  int i;
  for (i=0;i<N;i++){
    //generate random coordinates.
    //rand() returns an integer between 0 and RAND_MAX
    //If we did rand()/RAND_MAX that would do integer division, which is not what we want. So so:
    x=1.0*rand()/RAND_MAX;
    y=1.0*rand()/RAND_MAX;
    //see if they happen to be in unit circle
    if (x*x+y*y < 1){
      //record success
      successes+=1;
    }
  }
  //A random dart should land in that circle with probability pi/4.  So, we can estmate pi as
  //pi_est =4*successes/numtrials
  //watch out!  C does integer division by default, so include a double here
  //with a 4.0 and not a 4 to tell the compiler you want a double division. 
  double pi_est=4.0*successes/N;
  printf("Using %d samples we got pi is approximately %lf\n",N,pi_est);
    
  return(0);
}
