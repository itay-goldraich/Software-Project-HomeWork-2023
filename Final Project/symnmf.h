#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"


int executeCommand(char *command, Matrix XMatrix);
int getCommand(int argc, char *argv[]) ;
int getFileName(int argc, char *argv[]) ;
int countCommas(const char* input) ;
int getVector(FILE *f) ;
int getXMatrix(FILE *f) ;
double* copyRow();
double distance(double* a, double* b, int len) ;
