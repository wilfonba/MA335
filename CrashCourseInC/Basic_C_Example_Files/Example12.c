#include <stdio.h>
#include <stdlib.h>

void main(){

  //printf is the most common way to print in C.
  //You can print a string (recall that strings have quotes around them). 
  printf("hello world");

  //That isn't great, because it doesn't print a newline at the end.  The newline character is \n.
  printf("hello world\n");

  //printf is powerful because it can insert variables into the output, and it will format the output of the variables.
  //Formatting is controlled via format specifiers.

  //The format specifier for an integer is %d. 
  printf("Here I am, printing an integer: %d\n",41);

  printf("Now, I'll print two integers: %d,%d\n", 41,42);

  //This won't work properly to print 6, because b is a double and you told printf that you were trying to print an int. 
  int a=6;
  double b=6;
  printf("%d\n",b);

  //We %lf says "print a double".  Formatting is default, unless we specify it!
  printf("%lf\n",b);

  //Number of decimal places is controlled with .<number of places>
  //So, print b with 2 decimal places is:
  printf("%.2lf\n",b);

  //Number of positions for whole part is controlled with <number of paces>.
  //So, print b, make sure there are 5 positions before decimal place is:
  printf("%5.16lf\n",b);

  //You can make it pad with zeros with a leading 0:
  //print with 5 whole number positions, padding with 0's if need be,
  //and use exactly two decimal places:
  printf("%05.2lf\n",b);
  printf("%05.2lf\n",12345.0);
  
  //Put a +- sign at the front:
  printf("%+05.2lf\n",b);
  

  


}
