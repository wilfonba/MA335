#include<stdio.h>
#include<stdlib.h>

typedef struct{
   char*  data;
   int   origin;
   int   dest;
   int   length;
}message;

  void main(){
    message m;
    m.data=(char*) malloc(10*sizeof(char));
    m.data[0]='H'; m.data[1]='i'; m.data[2]='\n';
    m.origin=1;
    m.dest=2;
    m.length=15;
    printf("%s",m.data);

//    message v[2];
    message* v = malloc(2*sizeof(message));
    v[0].length=10;
    v[1].dest=3;
    v[1].data=malloc(5*sizeof(char));
    (v+1)->data[0]='H';
    (v+1)->data[1]='i';
    (v+1)->data[2]='2';
    (v+1)->data[3]='\n';
    printf("%s",v[1].data);
  }
