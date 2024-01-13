#define _GNU_SOURCE
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"


int vectorSize = 0;
int rowSize = 0;
double** XMatrix =NULL; 
double* Row = NULL; 

int executeCommand(char *command, Matrix* XMatrix);
char* getCommand(int argc, char *argv[]) ;
char* getFileName(int argc, char *argv[]) ;
int countCommas(const char* input) ;
int getVector(FILE *f) ;
int getXMatrix(FILE *f) ;
double* copyRow();
double distance(double* a, double* b, int len) ;

int main(int argc, char *argv[]) {
    
    char* command;
    char* fileName;
    FILE *f;
    Matrix* XMatrix2;
    int i,check;
    int x=0;
    x++;
    command = getCommand(argc, argv);
    if(command == NULL)
    {
        return 1;
    }
    fileName = getFileName(argc, argv);
    if(fileName == NULL)
    {
        return 1;
    }
    f = fopen(fileName,"r");
    if(f == NULL)
    {
        return 1;
    }
    check = getXMatrix(f);
    if(check == 1){
        fclose(f);
        printf("An Error Has Occurred1\n");
        return 1;
    }
    
    if(XMatrix == NULL)
    {
        fclose(f);
        printf("An Error Has Occurred2\n");
        return 1;
    }
    XMatrix2 = DoubleToMatrix(XMatrix, rowSize, vectorSize);
    if(XMatrix2 == NULL)
    {
        fclose(f);
        for (i = 0; i < rowSize; i++) {
            free(XMatrix[i]);
        }
        free(XMatrix);
        if(Row != NULL){
            free(Row);
        }
        printf("An Error Has Occurred2\n");
        return 1;
    }
    x = executeCommand(command, XMatrix2);

    fclose(f);
    for (i = 0; i < rowSize; i++) {
            free(XMatrix[i]);
        }
        free(XMatrix);
        if(Row != NULL){
            free(Row);
        }
        if(x == 1)
        {
            return 1;
        }
        free(XMatrix2);
        return 0;
}

int executeCommand(char *command, Matrix* XMatrix){
    Matrix* AMatrix;
    Matrix* DMatrix;
    Matrix* WMatrix;
    if(strcmp(command,"sym")==0){
        AMatrix = calculateSimilarityMatrix(XMatrix);
        if(AMatrix==NULL){
            return 1;
        }
        printMatrix(AMatrix);
        freeMatrix(AMatrix);
    }
    if(strcmp(command,"ddg")==0){
        AMatrix = calculateSimilarityMatrix(XMatrix);
        if(AMatrix==NULL){
            return 1;
        }
        DMatrix = calculateDiagonalDegreeMatrix(AMatrix);
        if(DMatrix==NULL){
            freeMatrix(AMatrix);
            return 1;
        }
        printMatrix(DMatrix);
        freeMatrix(AMatrix);
        freeMatrix(DMatrix);
    }
    if(strcmp(command,"norm")==0){
        WMatrix = calculateNormalizedSimilarityMatrix(XMatrix);
        if(WMatrix==NULL){
            return 1;
        }
        printMatrix(WMatrix);
        freeMatrix(WMatrix);
    }
    return 0;
}

char* getCommand(int argc, char *argv[]) {
    char* res;
    
    if (argc < 2) {
        return NULL;
    }
    res = argv[1];
    return res;
}

char* getFileName(int argc, char *argv[]) {
    char* res;
    
    if (argc < 2) {
        return NULL;
    }
    res = argv[2];
    return res;
}

int countCommas(const char* input) {
    int count = 0;
    while (*input != '\0') {
        if (*input == ',') {
            count++;
        }
        input++;
    }
    
    return count;
}

int getVector(FILE *f) {
    char* line = NULL;
    size_t bufsize = 0;
    char* start;
    char* end;
    int index;

    if (getline(&line, &bufsize, f) == -1) {
        if (Row != NULL) {
            free(Row);
            Row = NULL;
        }
        free(line);
        return 0;
    }

    if (vectorSize == 0) {
        vectorSize = countCommas(line) + 1;
    }
    
    if (Row != NULL) {
        free(Row);
    }

    Row = (double*)malloc(vectorSize * sizeof(double));
    if (Row == NULL) {
        free(line);
        return 1;
    }

    start = line;
    end = line;
    index = 0;
    while (*end != '\0' && index < vectorSize) {
        if (*end == ',') {
            *end = '\0';
            Row[index] = atof(start);
            start = end + 1;
            index++;
        }

        end++;
    }

    Row[index] = atof(start);
    free(line);
    return 0;
}


int getXMatrix(FILE *f) {
    int initialCapacity = 10;
    int capacity = initialCapacity;
    double* row;
    int flag;
    int i;
    double** resizedMatrix = NULL;
    XMatrix = (double**)malloc(initialCapacity * sizeof(double*));
    if (XMatrix == NULL) {
        return 1;
    }

    flag = getVector(f);
    if (flag == 1) {
        free(XMatrix);
        XMatrix = NULL;
        return 1;
    }

    while (Row != NULL) {
        if (rowSize == capacity) {
            capacity *= 2;
            resizedMatrix = (double**)realloc(XMatrix, capacity * sizeof(double*));
            if (resizedMatrix == NULL) {
                for (i = 0; i < rowSize; i++) {
                    free(XMatrix[i]);
                }
                free(XMatrix);
                XMatrix = NULL;
                return 1;
            }
            XMatrix = resizedMatrix;
        }

        row = copyRow();
        if (row == NULL) {
            for (i = 0; i < rowSize; i++) {
                free(XMatrix[i]);
            }
            free(XMatrix);
            XMatrix = NULL;
            if (Row != NULL) {
                free(Row);
                Row = NULL;
            }
            return 1;
        }

        XMatrix[rowSize] = row;
        rowSize++;
        flag = getVector(f);

        if (flag == 1) {
            for (i = 0; i < rowSize; i++) {
                free(XMatrix[i]);
            }
            free(XMatrix);
            XMatrix = NULL;
            if (Row != NULL) {
                free(Row);
                Row = NULL;
            }
            return 1;
        }
    }

    resizedMatrix = (double**)realloc(XMatrix, rowSize * sizeof(double*));
    if (resizedMatrix == NULL) {
        printf("An Error Has Occurred here\n");
        free(XMatrix);
        XMatrix = NULL;
        return 1;
    }

    XMatrix = resizedMatrix;
    return 0;
}


double* copyRow(){
    int i;
    double* copy = (double*)malloc(vectorSize * sizeof(double));
    if(copy == NULL){
        return copy;
    }
    for(i=0;i<vectorSize;i++){
        copy[i] = Row[i];
    }
    return copy;
}


double distance(double* a, double* b, int len) {
    double sum = 0;
    int i;
    for (i = 0; i < len; i++) {
        sum += pow(a[i] - b[i], 2);
    }
    return sqrt(sum);
}