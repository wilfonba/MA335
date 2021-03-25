#include<stdio.h>
#include<stdlib.h>

struct message{
   char*  data;
   int   origin;
   int   dest;
   int   length;
};

void print_message(struct message mess){
  printf("Message from: %d to: %d contents: %s\n",mess.origin, mess.dest,mess.data);
}

void main(){
    struct  message m;
    m.data=(char*) malloc(10*sizeof(char));
    m.data[0]='H'; m.data[1]='i'; m.data[2]='\n'; m.data[3]='\0';
    m.origin=1;
    m.dest=2;
    m.length=15;
    print_message(m);
  }
