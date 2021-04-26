#include <string.h>

char* int_arr2str(int* arr, int n) {
  int i;
  int max_digits=10000;
  char* intstr=malloc(max_digits*sizeof(char));

  int val;
  int final_str_cap=2;
  char* final_str=malloc(final_str_cap*sizeof(char));
  final_str[0]='['; final_str[1]='\0';

  for (i=0;i<n;i++) {
    val=snprintf(intstr,max_digits,"%d",arr[i]);
    if (val<0 || val>=max_digits) {
      fprintf(stderr,"Error writing a string.\n");
    }
    if (strlen(final_str)+strlen(intstr)+10>final_str_cap) {
      final_str=realloc(final_str,2*(strlen(final_str)+strlen(intstr)+10)*sizeof(char));
      final_str_cap=strlen(final_str)+strlen(intstr)+10;
    }
    strcat(final_str,intstr);
    if (i<n-1) {  
      strcat(final_str,",");
    }
  }
  strcat(final_str,"]");
  free(intstr);
  return(final_str);
}

char* dbl_arr2str(double* arr, int n) {
  int i;
  int max_digits=10000;
  char* dblstr=malloc(max_digits*sizeof(char));

  int val;
  int final_str_cap=2;
  char* final_str=malloc(final_str_cap*sizeof(char));
  final_str[0]='['; final_str[1]='\0';

  for (i=0;i<n;i++) {
    val=snprintf(dblstr,max_digits,"%.2lf",arr[i]);
    if (val<0 || val>=max_digits){
      fprintf(stderr,"Error writing a string.\n");
    }
    if (strlen(final_str)+strlen(dblstr)+10>final_str_cap) {
      final_str=realloc(final_str,2*(strlen(final_str)+strlen(dblstr)+10)*sizeof(char));
      final_str_cap=strlen(final_str)+strlen(dblstr)+10;
    }
    strcat(final_str,dblstr);
    if (i<n-1) {  
      strcat(final_str,",");
    }
  }
  strcat(final_str,"]");
  free(dblstr);
  return(final_str);
}
