#include<stdio.h>

void mod2mod3(int j,int* jmod2,int* jmod3){
    *jmod2=j % 2;
    *jmod3=j % 3;
}

int main(){
   int k=100;
   int kmod2; //memory has been allocated
   int kmod3; //for kmod2 and kmod3, but
              //there has been no value put 
              //in that memory, so it is 
              //garbage.
   mod2mod3(k,&kmod2,&kmod3); //give the function the
   //address of kmod2 and kmod3.The function will put the 
   //correct answer in those addresses. 
                             
   printf("%d \% 2= %d\n",k,kmod2);
   printf("%d \% 3= %d\n",k,kmod3);
}
