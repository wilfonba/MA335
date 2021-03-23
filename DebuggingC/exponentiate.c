#include<stdio.h>
#include<stdlib.h>

int ComputeFactorial(int number) {
  int fact = 0;
  int j;
  for (j = 1; j <= number; j++) {
    fact = fact * j;

  }

  return fact;
}

double power(double x, int n){
  int k;
  double xton=1;
  for (k=1;k<=n;k++){
    xton*=x;
  }
  return x;
}

double ComputeSeriesValue(double x, int n) {
  double seriesValue = 1.0;

  int k;
  for (k = 1; k <= n; k++) {
    seriesValue += power(x,k) / ComputeFactorial(k);
  }

  return seriesValue;
}

int main() {
  printf("This program is used to compute the value of the following series : \n");

  printf("1 + (x^1)/1! + (x^2)/2! + (x^3)/3! + (x^4)/4! + ........ + (x^n)/n!\n ");

  printf("Please enter the value of x : " );
  
  double x;
  scanf("%lf",&x);

  int n;
  printf("\nPlease enter an integer value for n : \n");
  scanf("%d",&n);
  printf("\n");


  double seriesValue = ComputeSeriesValue(x, n);
  printf("The value of the series for the values entered is %lf \n",seriesValue); 

  return 0;
}
