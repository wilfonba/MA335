typedef struct options{
  int maxn;           //fine all the primes between 2 and maxn
  int printresults;   //print the results or not
  int numprocs;       //the number of processes that are running
  int numworkers;     //the number of workers that are running
  int taskchunksize;
  int print_timing;
}options;


//prints an integer array
void print_int_arr(int* arr, int n){
  int i;
  int max_digits=10000;
  char* intstr=malloc(max_digits*sizeof(char));

  int val;
  int final_str_cap=2;
  char* final_str=malloc(final_str_cap*sizeof(char));
  final_str[0]='['; final_str[1]='\0';

  for (i=0;i<n;i++){
    val=snprintf(intstr,max_digits,"%d",arr[i]);
    if (val<0 || val>=max_digits){
      fprintf(stderr,"Error writing a string.\n");
    }
    if (strlen(final_str)+strlen(intstr)+10>final_str_cap){
      final_str=realloc(final_str,2*(strlen(final_str)+strlen(intstr)+10)*sizeof(int));
      final_str_cap=strlen(final_str)+strlen(intstr)+10;
    }
    strcat(final_str,intstr);
    if (i<n-1){  
      strcat(final_str,",");
    }
  }
  strcat(final_str,"]\n");
  printf(final_str);
  free(final_str);
  free(intstr);
}

//prints out an options struct
void print_opts(options* o){

  printf("N=2..%d\n",o->maxn);
  printf("taskchunksize: %d\n",o->taskchunksize);
  printf("Print Results: %s\n", o->printresults!=0? "Yes":"No");
  printf("Number of processors: %d\n",o->numprocs);
  printf("Number of workers: %d\n",o->numworkers);
}

//Takes the command line arguments and fills up the proper options in the provided options struct. 
void set_opts(int argc, char** argv, options* o){
  int i;
  int dummyllint;
  int dummyint;
  o->maxn=1000;
  o->printresults=0;
  o->print_timing=0;
  o->taskchunksize=50;
  MPI_Comm_size(MPI_COMM_WORLD,&dummyint);
  o->numprocs=dummyint;
  o->numworkers=o->numprocs-1;
  int forcestatic=0;
  for (i=0;i<argc;i++){
    fflush(stdout);
    if(sscanf(argv[i],"--printresults=%d",&dummyint)==1){
      o->printresults=dummyint;
    }
    if (sscanf(argv[i],"--taskchunksize=%d",&dummyllint)==1){
      o->taskchunksize=dummyllint;
    }
    if (strcmp(argv[i],"--forcestatic")==0){
      forcestatic=1;
    }
    if (strcmp(argv[i],"--print_timing")==0){
      o->print_timing=1;
    }
    else if (sscanf(argv[i],"%d",&dummyllint)==1){
      o->maxn=dummyllint;
    }
    
  }

  if (o->maxn<=1000){
    o->printresults=1;
  }

  if (forcestatic){
    o->taskchunksize=o->maxn/o->numworkers;
  }

 
  
}

