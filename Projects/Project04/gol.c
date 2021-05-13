#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

#include "gol_helpers.h"
#include "gol_comm_helpers.h"

//For reference, gol_helpers contains the following struct:
/*typedef struct{
  int*** state;
  int n_rows;
  int n_cols;
  int n_gens;
  }gol_board;*/







int main(int argc,char** argv){
  MPI_Init(&argc,&argv);
  gol_board g;
  options o; options defaults;

  int myrank;
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
  
  //Read options from prompt, fill in defaults
  strcpy(defaults.inputfile,"gol_files/pulsar.txt");
  defaults.resolution[0]=512; defaults.resolution[1]=512;
  defaults.n_gens=100;
  //Parse the command line, use "defaults" for default values.
  o=parse_commandline(argc,argv,defaults);

  //print the options that we are using:
  if (myrank==0){
    print_options(o); 
  }

  //Load the initial board -- note that every rank is doing this.  That's wasteful for sure, but makes thinkgs easier.
  initialize_board(o.inputfile,&g,o.n_gens);
  if (myrank==0){
    printf("Loaded %d x %d board\n",g.n_rows,g.n_cols);
  }

  

  /* //this line dumps the board to a file specified in the options.  */
  if(myrank==0){
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
