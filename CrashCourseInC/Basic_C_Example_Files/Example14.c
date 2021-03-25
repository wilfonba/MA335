#include <stdio.h>
#include <stdlib.h>

void main(){

  //line will hold the line typed in
  //number is the parsed input. 
  double number;
  char line[1000];

  printf("Enter a number:");
  //read from the command line (stdin)
  //read no more than 1000 characters (because that's the length of the line variable).
  //put the result in the line variable.
  //line is an array, so no need for &.
  fgets(line,1000,stdin);

  //parse line into a double.  Note the &, because number is not an array.
  sscanf(line,"%lf",&number);

  //Here is an if statement.  It is pretty expected.

  if (number<0){
    printf("Your number,%lf, is negative\n",number);
  }
  else{
    printf("Your number, %lf, is non-negative\n",number);
  }

    
  
}
