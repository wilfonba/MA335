#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv){
 
    char name[1000];
    char game[1000];
    printf("State your name:\n");
    scanf("%s",name);
    printf("State your business:\n");
    scanf("%s",game);
    printf("%s is the name and %s is the game.\n",name,game);
  }
