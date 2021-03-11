#include <stdio.h>
#include <stdlib.h>

void main(){
  //Integers work about like they should.
  int a;
  a=6;
  printf("%d\n",a);

  //doubles and floats work normally. 
  double b;
  b=3.14159;
  printf("%lf\n",b);

  //Decimal places don't belong in ints!
  a=2.718281828; 
  printf("%d\n",a);
  printf("%lf\n",a);

  //But assigning an integer to a non-integer type makes fine sense.
  double d;
  d=18;
  printf("%lf\n",d);

  //Characters are indicated using single quotes -- not double quotes!
  char c = 'a';
  printf("%c\n",c);

  //Arrays can be initialized using {}.
  int A[3]={1,2,3};
  printf("[%d,%d,%d]\n",A[0],A[1],A[2]);
  
  //Of course, we could also do assignment one term at a time.
  double B[3];
  B[0]=1; B[1]=1; B[2]=2;
  printf("[%lf,%lf,%lf]\n",B[0],B[1],B[2]);

  //Most common is to do it in  loops
  int C[10];
  int i;
  for (i=0; i<10;i++){
    C[i]=i;
  }
  printf("[%d,%d,%d]\n",C[0],C[1],C[2]);


  //Strings are arrays of characters.  So, here is one way to initialize a string.
  char str1[5]={'h','e','l','l','o'};

  //We can print it out like this:
  printf("%c%c%c%c%c\n",str1[0],str1[1],str1[2],str1[3],str1[4]);
  
  //C can be a pain, but even C knows that the above is a bad idea.
  //printf has the %s format specifier for printing arrays of characters.
  printf("%s\n",str1);

  //In order to know where the end of a string is, we must null-terminate character arrays.  The null terminator is the special symbol \0.
  char str2[6]={'h','e','l','l','o','\0'};
  printf("%s\n",str2);

  
  //A nicer way to initialize strings is with quotes.  Notice no need to include null terminator.  
  char str3[20]="hello world";
  printf("%s\n",str3);

}
