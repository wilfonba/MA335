#include<stdio.h>
#include<stdlib.h>

void main(){

  //gets will read an entire line of input
  char line[1000];
  char name[1000];
  
  printf("What is your name?\n");
  //get a line, from stdin (terminal), at most 1000 characters. Put the result in the line array.
  fgets(line,1000,stdin); 
  printf("Hi there, %s, nice to meet you!\n",line); 
  
  //That's annoying, the line includes the newline character I typed.
  //Parse it out with sscanf.
  sscanf(line,"%s",name);
  printf("Hi there, %s, nice to meet you!\n",name);
  
  
  //sscanf is great for parsing formatted input
  //Read three integers that are separated by commas. */
  int a; int b; int c;
  printf("Enter three numbers separated by commas\n");
  //get the line
  fgets(line,1000,stdin);
  //parse the line
  sscanf(line,"%d,%d,%d",&a,&b,&c);
  printf("Check out this calculation: (%d-%d+11)*%d=%d\n",a,b,c,(a-b+11)*c); 

  //Or, picking out part of input:
  printf("Type the sentence: 'My name is <your name here>'\n"); 
  fgets(line,1000,stdin);
  //Parse out the part we want:
  sscanf(line,"My name is %s",name);
  printf("Howdy, %s!\n",name); 
  

}
