#include<string.h>
#include<stdlib.h>
#include<string.h>
#include<stdio.h>

void make_int_array_str(int* arr, int len, char** str,int maxprintlen){
  *str=malloc(10000*sizeof(char));
  int str_len=0;
  int str_cap=10000;
  (*str)[0]='\0';
  if (arr==NULL){
    sprintf(*str,"NULL");
  }
  else if (len<=maxprintlen){
    char* int_str=malloc(1000*sizeof(char));;
    strcat(*str,"[");
    str_len=1;

    for (int i=0;i<len;i++){
      if (str_len>=str_cap-100){
	*str=realloc(*str,str_cap*2*sizeof(char));
	str_cap*=2;
      }
      sprintf(int_str,"%d",arr[i]);
      strcat(*str,int_str);
      if (i<len-1){
	strcat(*str,",");
      }
      else{
	strcat(*str,"]");
     
      }
      str_len+=strlen(int_str)+1;
    }
  }
  else{
    sprintf(*str,"[array with %d entries]",len);
  }

}
