#include <iostream>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>
#include "timer.h"

float** create_matrix( int numrows, int numcols, bool verbose){
	float *buffer=new float[numrows*numcols];
	float **data=new float*[numrows];
	for(int i=0;i<numrows;++i) data[i]=buffer+i*numcols;

	return *&data;
}

int main(int argc, char* argv[])
{
  bool verbose = true, definition = true;
  int i,j,rank=0,lines;
  FILE *file;
  float **matrixA,**matrixQ,**mat,**p;
  float *vec;
  arg::cTimer timer;

  if(rank != 0) verbose = false;

  if(rank == 0){
    for(i = 0; i < argc; i++){
      if(strcmp(argv[i],"-silent") == 0) verbose = false;
      if(strcmp(argv[i],"-file") == 0){
        if((file=fopen(argv[++i], "r")) == NULL) {
          printf("Cannot open file.\n");
          return(0);
        }
        else{
          definition = false;
        }
      }
      if(strcmp(argv[i],"-n") == 0){
        omp_set_num_threads(atoi(argv[++i]));
      }
    }

    if(definition){
      printf("Not enought input parameters.\nUse call like: ./program -n 1 -file file_name [-silent]\n");
      return(0);
    }

    fscanf(file,"%d,\n",&lines);
  }

  matrixA = create_matrix(lines,lines, verbose);
  matrixQ = create_matrix(lines,lines, verbose);
  for(int i = 0; i < lines; i++){
    matrixQ[i][i] = 1;
  }
  if(rank == 0){

    for(i = 0; i < lines; i++){
      for(j = 0; j < lines; j++){
        fscanf(file,"%f,",&matrixA[i][j]);
      }
    }
    if(verbose) printf("matrix was sucesfuly loaded to memory.\nMartix:\n");
    for(i = 0; i < lines; i++){
      for(j = 0; j < lines; j++){
        if(verbose) printf("%f ",matrixA[i][j]);
      }
      if(verbose) printf("\n");
    }

    timer.CpuStart();
  }

  int k,l,m;

  mat = create_matrix(lines,lines, false);

  for(i = 0; i < lines; i++){


    float x = 0;
    vec = new float[lines-i];
    for(j = i; j < lines; j++){
      vec[j-i] = -matrixA[j][i];
      x += vec[j-i]*vec[j-i];
    }
    x = sqrt(x);

    if(vec[0] > 0) x = -x;
    vec[0] = vec[0] + x;
    x = 0;
    for(j = 0; j < lines-i; j++){
      x += vec[j]*vec[j];
    }
    x = sqrt(x);

    if(x > 0){
      for(j = 0; j < lines-i; j++){
        vec[j] /= x;
      }

      p = create_matrix(lines-i,lines-i, false);
      for(k = 0; k < lines-i; k++){
        #pragma omp parallel for
        for(l = 0; l < lines-i; l++){
          if(k == l) p[k][k] = 1 - 2*vec[k]*vec[l];
          else p[k][l] = -2*vec[k]*vec[l];
        }
      }

      float tm;
      for(k = i; k < lines; k++){
        #pragma omp parallel for private(tm,m)
        for(l = i; l < lines; l++){
          tm = 0;
          for(m = i; m < lines; m++){
            tm += p[k-i][m-i]*matrixA[m][l];
          }
          mat[k][l] = tm;
        }
      }
      for(k = i; k < lines; k++){
        #pragma omp parallel for
        for(l = i; l < lines; l++){
          matrixA[k][l] = mat[k][l];
        }
      }

      for(k = 0; k < lines; k++){
        #pragma omp parallel for private(tm,m)
        for(l = i; l < lines; l++){
          tm = 0;
          for(m = i; m < lines; m++){
            tm += matrixQ[k][m]*p[m-i][l-i];
          }
          mat[k][l] = tm;
        }
      }
      for(k = 0; k < lines; k++){
        #pragma omp parallel for
        for(l = i; l < lines; l++){
          matrixQ[k][l] = mat[k][l];
        }
      }
      if(verbose) printf("Matrix Q in %d round.\n",i);
      for(k = 0; k < lines; k++){
        for(j = 0; j < lines; j++){
          if(verbose) printf("%f ",matrixQ[k][j]);
        }
        if(verbose) printf("\n");
      }
    }

  }
    double time;
    time = timer.CpuStop().CpuSeconds();

    if(verbose){
      printf("\nSolution is:\n");
      if(verbose) printf("Matrix Q.\n");
      for(k = 0; k < lines; k++){
        for(j = 0; j < lines; j++){
          if(verbose) printf("%f ",matrixQ[k][j]);
        }
        if(verbose) printf("\n");
      }
      printf("\n");
      if(verbose) printf("Matrix R.\n");
      for(k = 0; k < lines; k++){
        for(j = 0; j < lines; j++){
          if(verbose) printf("%f ",matrixA[k][j]);
        }
        if(verbose) printf("\n");
      }
    }

    if(verbose) printf("Time: ");
    printf("%f\n",time);

  return(0);
}
