void getNeighboringEdges(int* leftEdge,int* nLeftEdge,int* rightEdge,int* nRightEdge,int* topEdge, int* nTopEdge,int* bottomEdge,int*nBottomEdge,int myRank,int bi,int bj,int M,int N) {
    int source;
    if (bi%2 == 0) {
        source = block_coords_to_rank(M,N,bi + 1,bj);
        MPI_Sendrecv(rightEdge,M,MPI_INT,source,0,nRightEdge,M,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
        source = block_coords_to_rank(M,N,bi - 1,bj);
        MPI_Sendrecv(leftEdge,M,MPI_INT,source,0,nLeftEdge,M,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
    }
    else {
        source = block_coords_to_rank(M,N,bi - 1,bj);
        MPI_Sendrecv(leftEdge,M,MPI_INT,source,0,nLeftEdge,M,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
        source = block_coords_to_rank(M,N,bi + 1,bj);
        MPI_Sendrecv(rightEdge,M,MPI_INT,source,0,nRightEdge,M,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
    }
    if (bj%2 == 0) {
        source = block_coords_to_rank(M,N,bi,bj + 1);
        MPI_Sendrecv(topEdge,N,MPI_INT,source,0,nTopEdge,N,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
        source = block_coords_to_rank(M,N,bi,bj - 1);
        MPI_Sendrecv(bottomEdge,N,MPI_INT,source,0,nBottomEdge,N,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
    }
    else {
        source = block_coords_to_rank(M,N,bi,bj - 1);
        MPI_Sendrecv(bottomEdge,N,MPI_INT,source,0,nBottomEdge,N,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
        source = block_coords_to_rank(M,N,bi,bj + 1);
        MPI_Sendrecv(topEdge,N,MPI_INT,source,0,nTopEdge,N,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
    }
}

void getNeighboringCorners(int* topEdge,int* nTLcorner,int* nTRcorner,int* bottomEdge,int* nBLcorner,int* nBRcorner,int myRank,int bi,int bj,int M,int N) {
    int source;
    if (bi%2 == 0) {
        source = block_coords_to_rank(M,N,bi+1,bj+1);
        MPI_Sendrecv(&topEdge[N],1,MPI_INT,source,0,nTRcorner,1,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
        source = block_coords_to_rank(M,N,bi-1,bj-1);
        MPI_Sendrecv(&bottomEdge[1],1,MPI_INT,source,0,nBLcorner,1,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
    }
    else {
        source = block_coords_to_rank(M,N,bi-1,bj-1);
        MPI_Sendrecv(&bottomEdge[1],1,MPI_INT,source,0,nBLcorner,1,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
        source = block_coords_to_rank(M,N,bi+1,bj+1);
        MPI_Sendrecv(&topEdge[N],1,MPI_INT,source,0,nTRcorner,1,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
    }
    if (bj%2 == 0) {
        source = block_coords_to_rank(M,N,bi+1,bj-1);
        MPI_Sendrecv(&topEdge[1],1,MPI_INT,source,0,nTLcorner,1,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
        source = block_coords_to_rank(M,N,bi-1,bj+1);
        MPI_Sendrecv(&bottomEdge[N],1,MPI_INT,source,0,nBRcorner,1,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
    }
    else {
        source = block_coords_to_rank(M,N,bi-1,bj+1);
        MPI_Sendrecv(&bottomEdge[N],1,MPI_INT,source,0,nBRcorner,1,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
        source = block_coords_to_rank(M,N,bi+1,bj-1);
        MPI_Sendrecv(&topEdge[1],1,MPI_INT,source,0,nTLcorner,1,MPI_INT,source,0,MPI_COMM_WORLD,NULL);
    }
}