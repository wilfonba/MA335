
typedef struct{
  int print_results;
  char* source_file;
  int* to_partition;
  int num_to_partition;
  int force_display_all_numbers;
  int chunksize;
}options;


void print_usage(FILE* f){
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if (rank==0){
    fprintf(f,"A program to calculate a Goldbach partition of a list of numbers\n");
    fprintf(f,"\n\n");
    fprintf(f,"Options:\n");
    fprintf(f,"--source_file=<filename>         indicates that the list of numbers to partition should be read from <filename>, one per line.\n");
    fprintf(f,"--range=<a>/<b>                  indicates that the list of numbers to partition is a,a+1,a+2,...,b-1,b\n");
    fprintf(f,"--max=<n>                        indicates that the list of numbers to partition is 1,2,3,...,n.  The default is 100.\n");
    fprintf(f,"--list <l1> <l2> <l3> ...        indicates that the list of numbers to partition is l1,l2,l3,...  Note that there is no =, and that list entries are separated by a space\n");
    fprintf(f,"--force_display_all              indicates that the list of numbers to partition should be printed, even if it is long.\n"); 
    fprintf(f,"--print_results=<integer>        indicates whether or not a partition of all even numbers >= 2 in the list should be printed.0 means no, nonzero means yes.  Defaults to 0 if the list of integers is longer than 100 entries, and 1 otherwise\n");
    fprintf(f,"--chunksize=<chunksize>          This is the work chunk size for dynamic work allocation.  Each worker should check <chunksize> elements of the list as part of one assignment.\n");
  }  
}

void print_options(options* op){
  printf("Source File: %s\n",op->source_file);
  printf("Number of integers to partition: %d\n",op->num_to_partition);
  printf("Integers to partition: ");
  if (op->num_to_partition>40 && !op->force_display_all_numbers){
    printf("[ %d integers ] (use --force_display_all to show)\n",op->num_to_partition);
  }
  else{
    int j;
    for (j=0;j<op->num_to_partition;j++){
      printf("%d ",op->to_partition[j]);
    }
    printf("\n");
  }
  printf("Display partitions: %s\n",op->print_results==0 ? "No":"Yes");

}


void parse_options(int argc, char** argv, options* op){

  op->print_results=2;
  op->source_file="";
  op->to_partition=NULL;
  op->num_to_partition=0;
  op->force_display_all_numbers=0;
  op->chunksize=50;
  
  int i;
  int dummyi;
  double dummyd1, dummyd2;
  char dummys[1000];
  int list_set=0;
  for (i=1;i<argc;i++){

    if (sscanf(argv[i],"--source_file=%s",dummys)==1){
      op->source_file=malloc(sizeof(char)*strlen(dummys));
      strcpy(op->source_file,dummys);
      if (list_set){
	fprintf(stderr,"This combination of arguments not supported.\n");
	MPI_Finalize();
	exit(0);
      }

      FILE* f=fopen(op->source_file,"r");
      if (f==NULL){
	fprintf(stderr,"Error opening file %s\n",op->source_file);
	MPI_Finalize();
	exit(0);
      }
      int capacity=1000;
      op->to_partition=malloc(capacity*sizeof(int));
      while (!feof(f)){
	if (op->num_to_partition==capacity){
	  op->to_partition=realloc(op->to_partition, 2*sizeof(capacity*sizeof(int)));
	  capacity*=2;
	}
	fscanf(f,"%d",op->to_partition+(op->num_to_partition));
	op->num_to_partition+=1;
      }
      
      list_set=1;
    }
    else if (sscanf(argv[i],"--range=%lf/%lf",&dummyd1,&dummyd2)==2){
      int range[2];
      range[0]=(int) dummyd1; range[1]=(int) dummyd2;
      if (list_set){
	fprintf(stderr,"This combination of arguments not supported.\n");
	MPI_Abort(MPI_COMM_WORLD,0);
      }
      int j;
      op->num_to_partition=range[1]-range[0]+1;
      op->to_partition=malloc(op->num_to_partition*sizeof(int));
      for (j=0;j<op->num_to_partition;j++){
	op->to_partition[j]=range[0]+j;
      }
      list_set=1;
    }
    else if (sscanf(argv[i],"--max=%lf",&dummyd1)==1){
      if (list_set){
	fprintf(stderr,"This combination of arguments is not supported.\n");
	MPI_Finalize();
	exit(0);
      }
      int j;
      op->num_to_partition=(int) dummyd1;
      op->to_partition=malloc(op->num_to_partition*sizeof(int));
      for (j=1;j<=op->num_to_partition;j++){
	op->to_partition[j-1]=j;
      }
      list_set=1;
    }
    else if(strcmp("--list",argv[i])==0){
      if (list_set){
	fprintf(stderr,"This combination of arguments is not supported.\n");
	MPI_Finalize();
	exit(0);
      }
      i++;
      int capacity=1000;
      op->to_partition=malloc(capacity*sizeof(int));
      op->num_to_partition=0;
      while (i<argc && sscanf(argv[i],"%lf",&dummyd1)==1){
	if (op->num_to_partition==capacity){
	  op->to_partition=realloc(op->to_partition,2*capacity*sizeof(int));
	  capacity*=2;
	}
	op->to_partition[op->num_to_partition]=(int) dummyd1;
	i++;
	op->num_to_partition+=1;
      }
      i--;
      list_set=1;
    }
    
    else if (sscanf(argv[i],"--print_results=%d",&dummyi)==1){
      op->print_results=dummyi;
    }
    else if (strcmp("--force_display_all",argv[i])==0){
      op->force_display_all_numbers=1;
    }
    else if (sscanf(argv[i],"--chunksize=%d",&dummyi)==1){
      op->chunksize=dummyi;
    }
    else if (strcmp("-h",argv[i])==0 || strcmp("?", argv[i])==0 || strcmp("--help",argv[i])==0){
      print_usage(stdout);
      MPI_Finalize();
      exit(0);
    }
    else{
      printf("Unknown option %s\n",argv[i]);
      print_usage(stderr);
      MPI_Finalize();
      exit(1);
    }
  }

  if (!list_set){
    op->num_to_partition=100;
    op->to_partition=malloc(op->num_to_partition*sizeof(int));
    int j;
    for (j=0;j<op->num_to_partition;j++){
      op->to_partition[j]=j+1;
    }
  }

  if (op->num_to_partition > 100 && op->print_results==2){
    op->print_results=0;
  }
  op->print_results= op->print_results==0? 0:1;
}
