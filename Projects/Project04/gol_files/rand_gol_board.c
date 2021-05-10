#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void print_usage(){
  printf("Usage:\n");
  printf("rand_gol_board outputfilename numrows numcolumns\n");

}

void parse_options(int argc, char** argv, int* m, int* n,char* outputfile){
  if (argc<4){
    print_usage();
    exit(1);
  }
  if (sscanf(argv[1],"%s",outputfile)!=1){
    fprintf(stderr,"Error reading output filename.  Got %s.\n",argv[1]);
    print_usage();
    exit(1);
  }

  if (sscanf(argv[2],"%d",m)!=1){
    fprintf(stderr,"Error getting number of rows.  Read %s\n", argv[2]);
    print_usage();
    
    exit(1);
  }

  if (sscanf(argv[3],"%d",n)!=1){
    fprintf(stderr,"Error getting number of columns.  Tead %s\n",argv[3]);
    print_usage();

    exit(1);
  }

}

void main(int argc,char** argv){
  int m,n;
  char outputfile[1000];
  parse_options(argc,argv,& m, &n,outputfile);
  printf("Creating random %dx%d initial state in file %s.\n",m,n,outputfile);
  int i,j;
  srand(time(NULL));
  FILE* fp=fopen(outputfile,"w");
  if (fp==NULL){
    fprintf(stderr,"Error opening %s for writing. \n",outputfile);
    exit(1);
  }
  fprintf(fp,"%d,%d\n",m,n);
  for (i=0;i<m;i++){
    for(j=0;j<n;j++){
      fprintf(fp,"%d ",rand()%2);
    }
    fprintf(fp,"\n");
  }
  fclose(fp);
}
