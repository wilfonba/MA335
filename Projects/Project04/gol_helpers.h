#ifndef GOL_HELPERS_H
#define GOL_HELPERS_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include<mpi.h>
#include"gifsave89.c"

long long int set_state_calls=0;

//A gol board tells you a) how many rows it has, b) how many columns it has
//c) how many generations there are.
//Finally, state is a 3-d array such that board.state[t][i][j] should be the state of cell
//(i,j) at generation t. 
typedef struct{
  int*** state;
  int n_rows;
  int n_cols;
  int n_gens;
}gol_board;

typedef struct options{
  char inputfile[100];   //The name of the file containing the initial state
  char outputfile[100];  //the name of the file to generate (gif)
  int resolution[2];     //The resolution of the gif to generate.
  int numframes;         //The number of frames in the animated gif
  int n_gens;            //The number of generations (including the initial one) to end up with.
  int n_procs;           //The number of processors in the world
}options;


//This code gets the state of cell (i,j) at generation t.  However, it supports wrapping around--
//so get_state(g,1,-1,2,) gets the state cell on the *top* of the board in column 2 during generation 1. 
int get_state(gol_board* g,int t,int i,int j){
  return g->state[t][(i+g->n_rows)%(g->n_rows)][(j+g->n_cols)%(g->n_cols)];
}


//Calculates how many neighbors of cell (i,j) are occupied in generation t. 
int num_occupied_nbrs(gol_board* g, int t, int i, int j){
  int r[8]={-1,-1,-1,0,0,1,1,1};
  int c[8]={-1,0,1,-1,1,-1,0,1};
  int num_occupied=0;
  for (int k=0;k<8;k++){
    num_occupied+=get_state(g,t,i+r[k],j+c[k]);
  }
  return num_occupied;
}

//Sets the state of cell (i,j) during generation t to the given value.
//supports wrapping around correctly.
void set_state(gol_board* g, int t, int i, int j,int val){
  set_state_calls++;
  g->state[t][(i+g->n_rows)%(g->n_rows)][(j+g->n_cols)%(g->n_cols)]=val;
}

//copies a chunk of a gol board, the part between min_row and max_row and min_col and max_col and generations min_gen to max_gen (inclusive), into a buffer.  the buffer must be pre-allocated by the caller.  The data is copied in generation major, then row major order.  Returns the number of ints that was put in the buffer (in ints) via bufsize.  If bufsize==NULL, this is ignored.

//Handles negative rows and columns correctly, by wrapping around. 
void copy_board_range_to_buffer(gol_board* g, int min_row, int max_row, int min_col, int max_col,int min_gen, int max_gen,int* buf, int* bufsize){
  int num_to_copy=(max_row-min_row+1)*(max_col-min_col+1)*(max_gen-min_gen+1);
  if (bufsize != NULL){
    *bufsize=num_to_copy;
  }
  int t, i,j;
  int numcopied=0;
  for (t=min_gen;t<=max_gen;t++){
    for(i=min_row;i<=max_row;i++){
      for (j=min_col;j<=max_col;j++){
	buf[numcopied]=g->state[t][(i+2*g->n_rows)%g->n_rows][(j+2*g->n_cols)%g->n_cols];
	numcopied++;
      }
    }
  }

}

//copies a buffer, assumed to be in generation major, then row major order, into a chunk of a gol board.  It is copied into the range of the board between min_row, max_row, min_col, max_col, and min_gen and max_den (inclusive).  Handles wrap-around properly. 
void copy_buffer_to_board_range(int* buffer,gol_board* g, int min_row, int max_row, int min_col,int max_col, int min_gen, int max_gen){
  int t,i,j;
  int num_copied=0;
  for (t=min_gen;t<=max_gen;t++){
    for (i=min_row;i<=max_row;i++){
      for (j=min_col;j<=max_col;j++){
	g->state[t][(i+2*g->n_rows)%g->n_rows][(j+2*g->n_cols)%g->n_cols]=buffer[num_copied];
	num_copied++;
      }
    }
  }
}

int max(int i, int j){
  return i>j?i:j;
}

int min(int i, int j){
  return i<j?i:j;
}


//displays an entire gol board in plain text
void print_gol_board(gol_board* g){
  int t,i,j;
  char symb;
  for (t=0;t<g->n_gens;t++){
    printf("State at time: %d\n",t);
    for (i=0;i<g->n_rows;i++){
      for (j=0;j<g->n_cols;j++){
	symb=get_state(g,t,i,j)==1 ? 'O':'E';
	printf("%c ",symb);
      }
      printf("\n");
    }
  }

}

//displays an entire gol board in plain text, with neighbor counts. 
void print_gol_board_annotated(gol_board* g){
  int t,i,j;
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  for (t=0;t<g->n_gens;t++){
    printf("rank %d State at time: %d\n",rank,t);
    for (i=0;i<g->n_rows;i++){
      for (j=0;j<g->n_cols;j++){
	
	printf("%c(%d) ",get_state(g,t,i,j)==1 ? 'O':'E',num_occupied_nbrs(g,t,i,j));
      }
      printf("\n");
    }
  }

}


//prints just one state f a gol board. 
void print_gol_board_state(gol_board* g,int t){
  int i,j;
  
  for (i=g->n_rows-1;i>=0;i--){
    for (j=0;j<g->n_cols;j++){
	printf("%c(%d) ",get_state(g,t,i,j)==1 ? 'O':'E',num_occupied_nbrs(g,t,i,j));
    }
    printf("\n");
  }

}


//prints the usage of this program. 
void printusage(options defaults){

  fprintf(stderr,"gol options\n");
  fprintf(stderr,"available options are:\n");
  fprintf(stderr,"-h,--help,-?                      Print this message\n");
  fprintf(stderr,"--inputfile or -i=<inputfile>     Use this input file.  Default is %s\n",defaults.inputfile);
  fprintf(stderr,"--outputfile or -o=<outputfile>   Use this output file. Default is to derive output filename from input filename.\n");
  fprintf(stderr,"--generations or -g=<g>           Use g generations. Default is %d.\n",defaults.n_gens);
  fprintf(stderr,"--resolution=mxn                  Resolution of output gif.  Example usage is\ngol --resolution 512x512\nDefault is %dx%d, or the resolution of the gol board, whichever is bigger.\n",defaults.resolution[0],defaults.resolution[1]);
  fprintf(stderr,"--numframes=n                     Use n frames for animated gif.  Default is one frame per timestep, but that may be too much.\n");

}

options parse_commandline(int argc,char** argv,options defaults){
  options o;
  strcpy(o.inputfile,defaults.inputfile);
  int output_override=0;
  o.n_gens=defaults.n_gens;
  o.resolution[0]=defaults.resolution[0];
  o.resolution[1]=defaults.resolution[1];
  int numframes_override=0;
  int resolution_override=0;
  int i;
  int dummyd1,dummyd2;
  char dummys[100];
  MPI_Comm_size(MPI_COMM_WORLD,&o.n_procs);
  for (i=1;i<argc;i++){
    if (strcmp(argv[i],"-h")==0||strcmp(argv[i],"--help")==0||strcmp(argv[i],"-?")==0){
      printusage(defaults);
      exit(0);
    }
    else if (sscanf(argv[i],"-i=%s",dummys)==1||sscanf(argv[i],"--inputfile=%s",dummys)==1){
      strcpy(o.inputfile,dummys);
    }
    else if (sscanf(argv[i],"-o=%s",dummys)==1||sscanf(argv[i],"--outputfile=%s",dummys)==1){
      strcpy(o.outputfile,dummys);
      output_override=1;
    }
    else if (sscanf(argv[i],"-g=%d",&dummyd1)==1||sscanf(argv[i],"--generations=%d",&dummyd1)==1){
      o.n_gens=dummyd1;
    }
    else if(sscanf(argv[i],"--resolution=%dx%d",&dummyd1,&dummyd2)==2){
      o.resolution[0]=dummyd1;o.resolution[1]=dummyd2;
    }
    else if (sscanf(argv[i],"--numframes=%d",&dummyd1)==1){
      o.numframes=dummyd1;
      numframes_override=1;
    }
    else{
      fprintf(stderr,"Error parsing option %s.\n\n\n",argv[i]);
      printusage(defaults);
      exit(1);
    }
  }

  if (!numframes_override){
    o.numframes=o.n_gens;
  }

  if (!output_override){
    char* lastdot=strrchr(o.inputfile,'.');
    if (lastdot==NULL){
      strcpy(o.outputfile,o.inputfile);
      strcat(o.outputfile,".gif");
    }
    else{
      size_t rootlen=lastdot-o.inputfile;
      //printf("rootlen = %d\n",rootlen);
      strncpy(o.outputfile,o.inputfile,rootlen);
      o.outputfile[rootlen]='\0';
      strcat(o.outputfile,".gif");
    }
  }

  if (!resolution_override){
    FILE* f=fopen(o.inputfile,"r");
    if (f==NULL){
      fprintf(stderr,"Error opening %s for reading\n",o.inputfile);
      MPI_Abort(MPI_COMM_WORLD,1);
    }

    int nr; int nc;
    if (fscanf(f,"%d,%d",&nr,&nc)!=2){
      fprintf(stderr,"Error reading inputfile %s.  Expected first line to give diminesions of board.\n",o.inputfile);
      MPI_Abort(MPI_COMM_WORLD,1);
    }

    o.resolution[0]=max(defaults.resolution[0],nr);
    o.resolution[1]=max(defaults.resolution[1],nc);
  }
      
  
  return o;
}

void print_options(options o){

  printf("Input File (initial state): %s\n",o.inputfile);
  printf("Output File: %s\n",o.outputfile);
  printf("N_Gens: %d\n",o.n_gens);
  printf("Resolution: %dx%d\n",o.resolution[0],o.resolution[1]);
  printf("Number of frames requested: %d\n",o.numframes);
  printf("Number of frames output: %d\n",(o.n_gens/(o.n_gens/o.numframes)));

}



void insert_rect(unsigned char * pixels, int m, int n, int top_left_r, int top_left_c, int bottom_right_r, int bottom_right_c,int colorindex){
  int i;
  for (i=top_left_r; i<=bottom_right_r;i++){

    memset(pixels+i*n+top_left_c, colorindex, bottom_right_c-top_left_c+1);

  }

}


void insert_v_line(unsigned char* pixels, int m, int n, int c, int t, int b, int thickness, int col){
  if (thickness>0) insert_rect(pixels,m,n,t,c,b,c+(thickness-1),col);

}

void insert_h_line(unsigned char* pixels, int m, int n, int r, int left, int right, int thickness, int col){
  //printf("hline in row %d\n",r);
  if (thickness>0)  insert_rect(pixels,m,n,r,left,r+(thickness-1),right,col);
}

void gol_board_to_gif(gol_board* g, options o){

  if (o.resolution[0]<g->n_rows || o.resolution[1]<g->n_cols){
    fprintf(stderr,"There is no way I can reasonably output this to a file.  You don't even have 1 pixel per cell.\n");
    exit(1);
  }
  int colortable[]={255,255,255,0,0,0,128,128,255,0,0,0,-1};
  int bgindex=4;
  unsigned char* pixels=calloc(o.resolution[0]*o.resolution[1],sizeof(unsigned char));
  int frameskip=max(g->n_gens/o.numframes,1);
  int i,j,t;

  unsigned char* gifimage=NULL;
  void* gsdata=NULL;
  gsdata = newgif((void**) &gifimage,o.resolution[0],o.resolution[1],colortable,bgindex);

  if (gsdata==NULL){
    fprintf(stderr,"Error creating the gif file. Abort\n");
    exit(2);
  }

  animategif(gsdata,1,0,-1,2);
  int* sqr_hieght=malloc(g->n_rows*sizeof(int));
  int* sqr_width=malloc(g->n_cols*sizeof(int));
  for (i=0;i<g->n_rows;i++){
    sqr_hieght[i]=o.resolution[0]/g->n_rows;
    if (i< (o.resolution[0] % g->n_rows)){
      sqr_hieght[i]++;
    }
  }
  for (i=0;i<g->n_cols;i++){
    sqr_width[i]=o.resolution[1]/g->n_cols;
    if (i<o.resolution[1] % g->n_cols){
      sqr_width[i]++;
    }
  }
  int sqr_top_row[g->n_rows*sizeof(int)];
  int sqr_bottom_row[g->n_rows*sizeof(int)];
  int sqr_left_col[g->n_cols*sizeof(int)];
  int sqr_right_col[g->n_cols*sizeof(int)];

  sqr_top_row[0]=0; sqr_left_col[0]=0;
  sqr_bottom_row[g->n_rows-1]=o.resolution[0]-1;
  sqr_right_col[g->n_cols-1]=o.resolution[1]-1;
  
  for (i=0;i<max(g->n_cols,g->n_rows);i++){
    if (i<g->n_cols)   sqr_right_col[i]=sqr_left_col[i]+sqr_width[i]-1;
    if (i<g->n_cols-1) sqr_left_col[i+1]=sqr_right_col[i]+1;
    if (i<g->n_rows)   sqr_bottom_row[i]=sqr_top_row[i]+sqr_hieght[i]-1;
    if (i<g->n_rows-1) sqr_top_row[i+1]=sqr_bottom_row[i]+1;
  }

  int hline_thickness=o.resolution[0]/(2*(g->n_rows-1))/10;
  int vline_thickness=o.resolution[1]/(2*(g->n_cols-1))/10;
  for (t=0;t<g->n_gens;t+=frameskip){
    memset(pixels,(unsigned char)0,o.resolution[0]*o.resolution[1]);

    for (i=0;i<g->n_rows;i++){
      for (j=0;j<g->n_cols;j++){
	if (i>0) insert_h_line(pixels, o.resolution[0], o.resolution[1], sqr_top_row[i], sqr_left_col[j], sqr_right_col[j], hline_thickness, 1);

	if (j>0) insert_v_line(pixels,o.resolution[0], o.resolution[1], sqr_left_col[j],sqr_top_row[i],sqr_bottom_row[i],vline_thickness,1);
	if (i<g->n_rows-1) insert_h_line(pixels, o.resolution[0], o.resolution[1], sqr_bottom_row[i]-(hline_thickness-1), sqr_left_col[j], sqr_right_col[j], hline_thickness, 1);
	if (j<g->n_cols-1) insert_v_line(pixels,o.resolution[0], o.resolution[1], sqr_right_col[j]-(vline_thickness-1),sqr_top_row[i],sqr_bottom_row[i],vline_thickness,1);
	if (get_state(g,t,i,j)==1) insert_rect(pixels,o.resolution[0],o.resolution[1],sqr_top_row[i]+hline_thickness,sqr_left_col[j]+vline_thickness,sqr_bottom_row[i]-hline_thickness,sqr_right_col[j]-vline_thickness,2);
      }
    }
    
    putgif(gsdata,pixels);
  }
  int nbytes=endgif(gsdata);

  FILE* fp = fopen(o.outputfile,"wb");
  if (fp==NULL){
    fprintf(stderr,"Error opening %s for writing\n",o.outputfile);
    exit(1);
  }
  fwrite(gifimage,sizeof(unsigned char),nbytes,fp);
  fclose(fp);
}



void initialize_board(char* filename, gol_board* g,int n_gens){
  FILE* fp=fopen(filename,"r");
  if (fp==NULL){
    fprintf(stderr,"Error opening %s for read.\n",filename);
    exit(1);
  }

  int i,j;
  if (fscanf(fp,"%d,%d",&g->n_rows,&g->n_cols)!=2){
    fprintf(stderr,"Error reading line 1 of %s.  Expected board size.\n",filename);
    exit(1);
  }

  g->n_gens=n_gens;
  g->state=malloc(g->n_gens*sizeof(int**));
  int t;

  for (t=0;t<n_gens;t++){
    g->state[t]=malloc(g->n_rows*sizeof(int*));
    for (i=0;i<g->n_rows;i++){
      g->state[t][i]=malloc(g->n_cols*sizeof(int));
    }
  }
  
  
  int symb;
  int lineno=0;
  for (i=0;i<g->n_rows;i++){
    lineno++;
    for (j=0;j<g->n_cols;j++){
      if (fscanf(fp,"%d",&symb)!=1){
	fprintf(stderr,"Error reading %s, line %d.\n",filename,lineno);
	exit(1);
      }
      if (symb==0){
	g->state[0][i][j]=0;
      }
      else if(symb==1){
	g->state[0][i][j]=1;
      }
      else{
	fprintf(stderr,"Error reading %s, line %d.  Got symbol %d and expected 1 or 0.\n",filename,lineno,symb);
	exit(1);
      }
    }
  }
  

}




#endif
