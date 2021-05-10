
//returns in M and N the dimensions of the block grid that should be used on an m by n board if we are using n_procs processors. 
void get_block_dims(int m, int n, int n_procs,int* M, int* N){
  //List all the divisors of n_procs;
  //List the resulting rectangle sizes if we use this particular grid of processors. 
  int i;
  int best_rect_found=m>n? m+1:n+1;
  int best_Pn=m+1;
  int best_Pm=n+1;
  int Pm; int Pn;
  int block_m;
  int block_n;
  int block_difference;
  for (i=1;i<=n_procs;i++){
    //if i divides n_procs, consider i as a possible number of block-rows;
    if (n_procs%i==0){
      //then the number of block columns is n_procs/i
      //the resulting rectangle dimensions would be (m/i)x(n/(nprocs/i))
      //and the dimension difference would be |(m/i)-(n/(nprocs/i))|
      //so,
      Pm=i;
      Pn=n_procs/i;
      block_m=m/Pm;
      if (m  % Pm != 0){
	block_m+=1;
      }
      block_n=n/Pn;
      if (n % Pn > 0){
	block_n+=1;
      }
      
      block_difference=abs(block_m-block_n);
      //printf("One option is P_m=%d, P_n=%d, which results in blocks of size %d x %d which have side difference %d\n",Pm,Pn,block_m,block_n,block_difference);

      if (block_difference<best_rect_found){
	best_Pm=Pm;
	best_Pn=Pn;
	best_rect_found=block_difference;
      }
    }
  }
  *M=best_Pm>m?m:best_Pm; *N=best_Pn>n?n:best_Pn;
}


//Gets the block coordinates, bi, bj that processor rank should work on, if the block grid is MxN
void get_block_coords(int M, int N, int rank, int* bi, int* bj){
 *bi=rank / (N);
 *bj=rank % (N);
}


//Takes some block coordinates and returns the processor that should be servicing that block.
//handles wrap-around correctly. 
int block_coords_to_rank(int M,int N, int bi, int bj){
  return (((bi+2*M)%M)*N+((bj+2*N)%N));
}


//Returns the leftmost column, the rightmost column, the bottom most row, and topmost row of the board that is covered by block bi,bj if the block grid is M x N and the board is m x n. 
void get_assignment(int m, int n, int M, int N, int bi, int bj, int* left_edge,int* right_edge, int* bottom_edge, int* top_edge){
  
  int le_block_col[N];
  int re_block_col[N];
  int be_block_row[M];
  int te_block_row[M];
  int block_width=n/N;
  int block_hieght=m/M;
  int i;
  be_block_row[0]=0;
  for (i=0;i<M-1;i++){
    te_block_row[i]=be_block_row[i]+block_hieght-1;
    if (i<m%M){
      te_block_row[i]+=1;
    }
    be_block_row[i+1]=te_block_row[i]+1;
  }
  te_block_row[M-1]=m-1;

  le_block_col[0]=0;
  for (i=0;i<N-1;i++){
    re_block_col[i]=le_block_col[i]+block_width-1;
    if (i<n%N){
      re_block_col[i]+=1;
    }
    le_block_col[i+1]=re_block_col[i]+1;
  }
  re_block_col[N-1]=n-1;

  //printf("Rank %d gets block row %d and block column %d\n",rank,my_block_row,my_block_col);
  //Finally, now you can get the coordinates of the cells that this rank will work on.
  if (bi<M && bj<N){
    *left_edge=le_block_col[bj];
    *right_edge=re_block_col[bj];
    *bottom_edge=be_block_row[bi];
    *top_edge=te_block_row[bi];
      }
  else{
    *left_edge=-1; *right_edge=-1; *bottom_edge=-1; *top_edge=-1;
  }
}

