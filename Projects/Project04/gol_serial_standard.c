#include <stdio.h>
#include <stdlib.h>


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



//I've written this code wierd.  Make sure to keep this #include *after* the gol_board declaration, *after* get_state, and *after* num_occupied_nbrs. 
#include "gol_serial_helpers.h"



//Fills in generation t+1 from generation t
void transition(gol_board* g, int t){
  int i; int j;
  int oc_nbrs;
  int mystatus,newstatus;
  //loop over every cell on the board
  for (i=0; i<g->n_rows; i++){
    for (j=0;j<g->n_cols;j++){
      //Get the state of this cell at generation t
      mystatus=get_state(g,t,i,j);
      //Calculate the number of neighbors of cell (i,j) that are occupied during at generation t
      oc_nbrs=num_occupied_nbrs(g,t,i,j);
      //Now set the status of cell (i,j) at generation t+1 accordingly. 
      if (mystatus==1){
	if (oc_nbrs==2 || oc_nbrs==3) newstatus=1;
	if (oc_nbrs>=4) newstatus=0;
	if (oc_nbrs<2) newstatus=0;
      }
      else if (mystatus==0){
	if (oc_nbrs==3) newstatus=1;
	else newstatus=0;
      }
      g->state[t+1][i][j]=newstatus;
    }

  }

}


int main(int argc,char** argv){

  gol_board g;
  options o; options defaults;
  //Create an options struct called defaults.  Set the default values for every option.
  strcpy(defaults.inputfile,"gol/pulsar.txt");
  defaults.resolution[0]=512; defaults.resolution[1]=512;
  defaults.n_gens=100;
  //Parse the command line, use "defaults" for default values. 
  o=parse_commandline(argc,argv,defaults);
  //Print the final set of options.
  print_options(o);
  //Now load up an initial state from the input file specified. 
  initialize_board(o.inputfile,&g,o.n_gens);
  printf("Loaded a %dx%d board.\n",g.n_rows,g.n_cols);
  int i;
 
  //Now fill in each generation of the board from the previous generation.  
  for (i=0;i<g.n_gens-1;i++){
    transition(&g,i);
  }
  //This line prints the board in plain text
  //print_gol_board(&g);
  //this line dumps the board to a file specified in the options. 
  gol_board_to_gif(&g,o);
  return (0);
}
