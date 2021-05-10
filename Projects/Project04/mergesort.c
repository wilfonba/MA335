#include <stdlib.h>
#include <string.h>
#ifndef __MERGESORT__
#define __MERGESORT__

void merge(double* S,double* left, int left_n, double* right, int right_n){
	int i; 
	int n=left_n+right_n;
	int left_top=0;
	int right_top=0;
	for (i=0;i<n;i++){
		if (left_top==left_n){
			S[i]=right[right_top];
			right_top++;
		}
		else if (right_top==right_n){
			S[i]=left[left_top];
			left_top++;
		}
		else if (left[left_top]<=right[right_top]){
			S[i]=left[left_top];
			left_top++;
		}
		else{
			S[i]=right[right_top];
			right_top++;
		}
	}

}

void mergesort(double* L,int n){
	if (n==1){
		return;
	}
	else{
		double* left=malloc(n/2*sizeof(double));
		memcpy(left,L,n/2*sizeof(double));
		double* right=malloc((n-n/2)*sizeof(double));
		memcpy(right,L+n/2,(n-n/2)*sizeof(double));
		mergesort(left,n/2);
		mergesort(right,n-n/2);
		merge(L,left,n/2,right,n-n/2);
		free(left);
		free(right);
	}

}

#endif