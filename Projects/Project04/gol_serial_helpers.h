#ifndef GOL_HELPERS_H
#define GOL_HELPERS_H

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<math.h>
#include"gifsave89.c"

typedef struct options{
  char inputfile[100];
  char outputfile[100];
  int resolution[2];
  int numframes;
  int n_gens;
}options;


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

void print_gol_board_annotated(gol_board* g){
  int t,i,j;
  
  for (t=0;t<g->n_gens;t++){
    printf("State at time: %d\n",t);
    for (i=0;i<g->n_rows;i++){
      for (j=0;j<g->n_cols;j++){
	
	printf("%c(%d) ",get_state(g,t,i,j)==1 ? 'O':'E',num_occupied_nbrs(g,t,i,j));
      }
      printf("\n");
    }
  }

}

void printusage(options defaults){

  fprintf(stderr,"gol options\n");
  fprintf(stderr,"available options are:\n");
  fprintf(stderr,"-h               Print this message\n");
  fprintf(stderr,"-i=inputfile     Use this input file.  Default is %s\n",defaults.inputfile);
  fprintf(stderr,"-o=outputfile    Use this output file. Default is to derive output filename from input filename.\n");
  fprintf(stderr,"-g=g             Use g generations. Default is %d.\n",defaults.n_gens);
  fprintf(stderr,"--resolution=mxn Resolution of output gif.  Example usage is\ngol --resolution 512x512\nDefault is %dx%d\n",defaults.resolution[0],defaults.resolution[1]);
  fprintf(stderr,"--numframes=n    Use n frames for animated gif.  Default is one frame per timestep, but that may be too much.\n");

}

options parse_commandline(int argc,char** argv,options defaults){
  options o;
  strcpy(o.inputfile,defaults.inputfile);
  int output_override=0;
  o.n_gens=defaults.n_gens;
  o.resolution[0]=defaults.resolution[0];
  o.resolution[1]=defaults.resolution[1];
  int numframes_override=0;
  int i;
  int dummyd1,dummyd2;
  char dummys[100];
  for (i=1;i<argc;i++){
    if (strcmp(argv[i],"-h")==0){
      printusage(defaults);
      exit(0);
    }
    else if (sscanf(argv[i],"-i=%s",dummys)==1){
      strcpy(o.inputfile,dummys);
    }
    else if (sscanf(argv[i],"-o=%s",dummys)==1){
      strcpy(o.outputfile,dummys);
      output_override=1;
    }
    else if (sscanf(argv[i],"-g=%d",&dummyd1)==1){
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


int max(int i, int j){
  return i>j?i:j;
}

int min(int i, int j){
  return i<j?i:j;
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

#endif
