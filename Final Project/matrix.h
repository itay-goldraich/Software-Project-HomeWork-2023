#include <stdio.h>
#include <stdlib.h>
#include <math.h>


typedef struct {
    int rows;
    int cols;
    double** data;
} Matrix;

Matrix* createMatrix(int rows, int cols);
void freeMatrix(Matrix* matrix);
double** GetData (Matrix* matrix);
Matrix* calculateSimilarityMatrix(Matrix* matrix);
double calcSquareInSimilarityMatrix(Matrix* matrix, int i, int j);
double NormalSquared(Matrix* matrix, int x, int y);
Matrix* calculateDiagonalDegreeMatrix(Matrix* matrix);
Matrix* calculateNormalizedSimilarityMatrix(Matrix* matrix);
Matrix* multiplyMatrices(Matrix* mat1, Matrix* mat2);
Matrix* transposeMatrix(Matrix* matrix);
void printMatrix(Matrix* matrix);
Matrix* PowMatirx (Matrix* matrix, double power);
Matrix* SubtractionMatrices(Matrix* mat1, Matrix* mat2);
double Frobeniusnorm(Matrix* matrix);
Matrix* RandomMatrix (double m, double k, int row, int col);
Matrix* UpdatematrixH (Matrix* matrixW, Matrix* matrixH);
double AvgOfMatrix (Matrix* matrix);
Matrix* DoubleToMatrix (double** data, int rows, int cols);
double FrobeniusnormSquared(Matrix* matrix);
