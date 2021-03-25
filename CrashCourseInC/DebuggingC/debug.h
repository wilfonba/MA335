#include<time.h>

#define debug(L,...) {\
    if (DEBUG_LEVEL>=L){\
      fprintf(stderr, "[DEBUG (%d)] %s:%d: ",L, __FILE__, __LINE__);	\
      fprintf(stderr,##__VA_ARGS__);				\
    }\
  }



void time_stamp(char * timestamp){

  time_t ltime;
  ltime=time(NULL);
  struct tm *tm;
  tm=localtime(&ltime);

  sprintf(timestamp,"%04d %02d %02d %02d:%02d:%02d", tm->tm_year+1900, tm->tm_mon, 
	  tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
}

