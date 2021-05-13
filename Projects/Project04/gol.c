#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>


#include "gol_helpers.h"
#include "gol_comm_helpers.h"
#include "my_gol_helpers.h"

//For reference, gol_helpers contains the following struct:
/*typedef struct{
  int*** state;
  int n_rows;
  int n_cols;
  int n_gens;
  }gol_board;*/

void playGol(options* o, gol_board* g,int myRank) {
  int M;
  int N;
  int bi;
  int bj;
  int* leftEdge = malloc(M*sizeof(int));
  int* rightEdge = malloc(M*sizeof(int));
  int* topEdge = malloc(N*sizeof(int));
  int* bottomEdge = malloc(N*sizeof(int));
  int* nLeftEdge = malloc(M*sizeof(int));
  int* nRightEdge = malloc(M*sizeof(int));
  int* nTopEdge = malloc(N*sizeof(int));
  int* nBottomEdge = malloc(N*sizeof(int));
  int nTLcorner;
  int nTRcorner;
  int nBLcorner;
  int nBRcorner;
  int i;
  get_block_dims(g->n_rows,g->n_cols,o->n_procs,&M,&N);
  get_block_coords(M,N,myRank,&bi,&bj);
  /*int minRow = (bi*M + g->n_rows)%g->n_rows;
  int maxRow = ((bi + 1)*M + g->n_rows - 1)%g->n_rows;
  int minCol = (bj*M + g->n_cols)%g->n_cols;
  int maxCol = ((bj + 1)*M + g->n_cols - 1)%g->n_cols;*/

  for (i = 0;i < g->n_gens;i++) {
    // Get my edges to send to other processors
    get_assignment(g->n_rows,g->n_cols,M,N,bi,bj,leftEdge,rightEdge,bottomEdge,topEdge);
    // Receiv edges from neighboring processors
    getNeighboringEdges(leftEdge,nLeftEdge,rightEdge,nRightEdge,topEdge,nTopEdge,bottomEdge,nBottomEdge,myRank,bi,bj,M,N);
    getNeighboringCorners(topEdge,&nTLcorner,&nTRcorner,bottomEdge,&nBLcorner,&nBRcorner,myRank,bi,bj,M,N);
  }
}


int main(int argc,char** argv){
  MPI_Init(&argc,&argv);
  gol_board g;
  options o; options defaults;

  int myRank;
  MPI_Comm_rank(MPI_COMM_WORLD,&myRank);
  
  //Read options from prompt, fill in defaults
  strcpy(defaults.inputfile,"gol_files/pulsar.txt");
  defaults.resolution[0]=512; defaults.resolution[1]=512;
  defaults.n_gens=100;
  //Parse the command line, use "defaults" for default values.
  o=parse_commandline(argc,argv,defaults);

  //print the options that we are using:
  if (myRank==0){
    print_options(o); 
  }

  //Load the initial board -- note that every rank is doing this.  That's wasteful for sure, but makes thinkgs easier.
  initialize_board(o.inputfile,&g,o.n_gens);
  if (myRank==0){
    printf("Loaded %d x %d board\n",g.n_rows,g.n_cols);
  }

  // Play GOL
  printf("calling playGol\n");fflush(stdout);
  playGol(&o,&g,myRank);






  /* //this line dumps the board to a file specified in the options.  */
  if(myRank==0) {
    //makes an animated gif out of the board stored in g using the options in o. 
    gol_board_to_gif(&g,o);
    //This command prints the gol board in plain text with neighbor counts
    //print_gol_board_annotated(&g);
    //This prints the bol board in plain text without neighbor counts
    //print_gol_board(&g)
  }
  MPI_Finalize();
  exit(0);
}
