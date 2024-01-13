#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "matrix.h"


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


Matrix* createMatrix(int rows, int cols) {
    Matrix* matrix =(Matrix*)malloc(sizeof(Matrix));
    int i,j;
    if(matrix == NULL){
        return NULL;
    }
    
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = (double**)malloc(rows * sizeof(double*));
    if(matrix->data == NULL){
        free(matrix);
        return NULL;
    }
    for (i = 0; i < rows; i++) {
        matrix->data[i] = (double*)malloc(cols * sizeof(double));
        if(matrix->data[i] == NULL){
            for(j=0;j<i;j++){
                free(matrix->data[j]);
                free(matrix);
                return NULL;
            }
        }
        for(j=0;j<cols;j++)
        {
            matrix->data[i][j]=0;
        }
    }
    return matrix;
}

void freeMatrix(Matrix* matrix) {
    int i;
    for (i = 0; i < matrix->rows; i++) {
        free(matrix->data[i]);
    }
    free(matrix->data);
    free(matrix);
}

double** GetData (Matrix* matrix){
    return matrix->data;
}

Matrix* calculateSimilarityMatrix(Matrix* matrix) {
    int i,j;
    Matrix* nematrixW = createMatrix(matrix->rows, matrix->rows);
    if(nematrixW == NULL){
        return NULL;
    }
    for (i = 0; i < matrix->rows; i++) {
        for (j = 0; j < matrix->rows; j++) {
            nematrixW->data[i][j] = calcSquareInSimilarityMatrix(matrix, i, j);
        }
    }
    return nematrixW;
}

double calcSquareInSimilarityMatrix(Matrix* matrix, int i, int j) {
    if (i == j) {
        return 0;
    }
    return exp(-1 * NormalSquared(matrix, i, j) / 2);
}

double NormalSquared(Matrix* matrix, int x, int y) {
    double result = 0;
    int i;
    for (i = 0; i < matrix->cols; i++) {
        result += pow(matrix->data[x][i] - matrix->data[y][i], 2);
    }
    return result;
}

Matrix* calculateDiagonalDegreeMatrix(Matrix* matrixX) {
    int i,j;
    Matrix* matrixD = createMatrix(matrixX->rows, matrixX->cols);
    if(matrixD == NULL){
        return NULL;
    }
    for (i = 0; i < matrixD->rows; i++) {
        for (j = 0; j < matrixD->cols; j++) {
            matrixD->data[i][i] += matrixX->data[i][j];
        }
    }
    return matrixD;
}

Matrix* calculateNormalizedSimilarityMatrix(Matrix* matrix) {
    Matrix* matrixA;
    Matrix* matrixD;
    Matrix* matrixD_half;
    Matrix* D_A;
    Matrix* D_A_D;
    matrixA = calculateSimilarityMatrix(matrix);
    if(matrixA == NULL){
        return NULL;
    }
    matrixD = calculateDiagonalDegreeMatrix(matrixA);
    if(matrixD == NULL){
        freeMatrix(matrixA);
        return NULL;
    }
    matrixD_half = PowMatirx(matrixD,-0.5);
    if(matrixD_half == NULL){
        freeMatrix(matrixA);
        freeMatrix(matrixD);
        return NULL;
    }
    D_A = multiplyMatrices(matrixD_half,matrixA);
    if(D_A == NULL){
        freeMatrix(matrixA);
        freeMatrix(matrixD);
        freeMatrix(matrixD_half);
        return NULL;
    }
    D_A_D = multiplyMatrices(D_A,matrixD_half);
    freeMatrix(matrixA);
    freeMatrix(matrixD);
    freeMatrix(matrixD_half);
    freeMatrix(D_A);
    return D_A_D;
}

Matrix* multiplyMatrices(Matrix* mat1, Matrix* mat2) {
    int rows1 = mat1->rows;
    int cols1 = mat1->cols;
    int cols2 = mat2->cols;
    int i,j,k;
    Matrix* nematrixW = createMatrix(rows1, cols2);
    if(nematrixW == NULL){
        return NULL;
    }
    for (i = 0; i < rows1; i++) {
        for (j = 0; j < cols2; j++) {
            nematrixW->data[i][j] = 0;
            for (k = 0; k < cols1; k++) {
                nematrixW->data[i][j] += mat1->data[i][k] * mat2->data[k][j];
            }
        }
    }
    return nematrixW;
}

Matrix* SubtractionMatrices(Matrix* mat1, Matrix* mat2) {
    int i,j;
    Matrix* nematrixW = createMatrix(mat1->rows, mat1->cols);
    if(nematrixW == NULL){
        return NULL;
    }
    for (i = 0; i < mat1->rows; i++) {
        for (j = 0; j < mat1->cols; j++) {
            nematrixW->data[i][j] = mat1->data[i][j] - mat2->data[i][j];
        }
    }
    return nematrixW;
}

double Frobeniusnorm(Matrix* matrix) {
    double sum = 0.0;
    int i,j;
    for (i = 0; i < matrix->rows; i++) {
        for (j = 0; j < matrix->cols; j++) {
            sum += fabs(matrix->data[i][j])*fabs(matrix->data[i][j]);
        }
    }
    return sqrt(sum);
}

double FrobeniusnormSquared(Matrix* matrix) {
    double sum = 0.0;
    int i,j;
    for (i = 0; i < matrix->rows; i++) {
        for (j = 0; j < matrix->cols; j++) {
            sum += fabs(matrix->data[i][j])*fabs(matrix->data[i][j]);
        }
    }
    return sum;
}


Matrix* PowMatirx (Matrix* matrix, double power){
    int i,j;
    Matrix* nematrixW = createMatrix(matrix->rows, matrix->cols);
    if(nematrixW == NULL){
        return NULL;
    }
    
    for (i = 0; i < matrix->rows; i++) {
        for (j = 0; j < matrix->cols; j++) {
            if(matrix->data[i][j]!=0){
                nematrixW->data[i][j] = pow(matrix->data[i][j],power);
            }
            
        }
    }
    return nematrixW;
}

Matrix* transposeMatrix(Matrix* matrix) {
    Matrix* nematrixW = createMatrix(matrix->cols, matrix->rows);
    int i,j;
    if(nematrixW == NULL){
        return NULL;
    }
    
    for (i = 0; i < nematrixW->rows; i++) {
        for (j = 0; j < nematrixW->cols; j++) {
            nematrixW->data[i][j] = matrix->data[j][i];
        }
    }
    return nematrixW;
}

void printMatrix(Matrix* matrix) {
    int i,j;
    for(i=0; i<matrix->rows; i++){
        for(j=0; j<matrix->cols; j++){
            if(j > 0 ){ printf(","); }
            printf("%.4f",matrix->data[i][j]);
        }
        printf("\n");
    }
}

Matrix* RandomMatrix (double m, double k, int rows, int cols){
    int i,j;
    Matrix* nematrixW = createMatrix(rows, cols);
    if(nematrixW == NULL){
        return NULL;
    }
    for (i = 0; i < nematrixW->rows; i++) {
        for (j = 0; j < nematrixW->cols; j++) {
            nematrixW->data[j][i] = (2.0 * sqrt(m / k)) * ((double)rand() / RAND_MAX);
        }
    }
    return nematrixW;
}


Matrix* UpdatematrixH (Matrix* matrixW, Matrix* matrixH){
    double BETA = 0.5;
    int i,j;
    Matrix *newmatrixH, *TmatrixH, *matrixWxH, *matrixH_TH,*matrixH_TH_H;
    newmatrixH = createMatrix(matrixH->rows, matrixH->cols);
    if(newmatrixH == NULL){
        return NULL;
    }
    TmatrixH = transposeMatrix(matrixH);
    if(TmatrixH  == NULL){
        freeMatrix(newmatrixH);
        return NULL;
    }
    matrixWxH = multiplyMatrices(matrixW,matrixH);
    if(matrixWxH  == NULL){
        freeMatrix(newmatrixH);
        freeMatrix(TmatrixH);
        return NULL;
    }
    matrixH_TH = multiplyMatrices(matrixH, TmatrixH);
    if(matrixH_TH  == NULL){
        freeMatrix(newmatrixH);
        freeMatrix(TmatrixH);
        freeMatrix(matrixWxH);
        return NULL;
    }
    matrixH_TH_H =multiplyMatrices(matrixH_TH,matrixH);
    if(matrixH_TH_H  == NULL){
        freeMatrix(newmatrixH);
        freeMatrix(TmatrixH);
        freeMatrix(matrixWxH);
        freeMatrix(matrixH_TH);
        return NULL;
    }
    for (i = 0; i < newmatrixH->rows; i++) {
        for (j = 0; j < newmatrixH->cols; j++) {
            newmatrixH->data[i][j] = matrixH->data[i][j]*(1-BETA + BETA*((matrixWxH->data[i][j])/(matrixH_TH_H->data[i][j])));
        }
    }

    freeMatrix(matrixWxH);
    freeMatrix(matrixH_TH);
    freeMatrix(matrixH_TH_H);
    freeMatrix(TmatrixH);
    return newmatrixH;
}


double AvgOfMatrix (Matrix *matrix){
    double sum = 0.0;
    int i,j;
    for (i = 0; i < matrix->rows; i++) {
        for (j = 0; j < matrix->cols; j++) {
            sum += matrix->data[i][j];
        }
    }
    return sum/(matrix->rows*matrix->cols);
}

Matrix* DoubleToMatrix(double** data, int rows, int cols) {
    Matrix *matrix =(Matrix*)malloc(sizeof(Matrix));
    if(matrix == NULL){
        return NULL;
    }
    matrix->rows = rows;
    matrix->cols = cols;
    matrix->data = data;
    return matrix;
}