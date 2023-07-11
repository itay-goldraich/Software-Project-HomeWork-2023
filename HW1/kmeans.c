#define _GNU_SOURCE
#define MAX_LINE_LENGTH 1000
#define DEFAULT_ITER 200
#define EPSILON 0.001

#include <stdio.h>
#include <stdlib.h>
#include <math.h>


int getK(int argc, char *argv[]);
int getIter(int argc, char *argv[]);
int getMatrix();
int getVector();
double distance(double* a, double* b, int len);
double** initialize(int k, double** AllVectors);
int printCentroids(double** centroids, int k,int len);
int k_means(double** AllVectors, int n, int len, int k, int max_iterations, double** centroids);
int mainWork(int k, int iter, double** AllVectors);
void printDoubleMatrix(int row);
double* copyRow();


int vectorSize = 0;
int rowSize = 0;
double* Row = NULL;
double** matrix = NULL;

int main(int argc, char *argv[]) {
    int k;
    int iter;
    int i;
    k = getK(argc, argv);
    if (k <= 1) {
        printf("Invalid number of clusters!\n");
        return 1;
    }
    iter = getIter(argc, argv);
    if (iter >= 1000 || iter <= 1) {
        printf("Invalid maximum iteration!\n");
        return 1;
    }
    if(getMatrix() == 1){
        
        printf("An Error Has Occurred\n");
        return 1;
    }
    if(matrix == NULL)
    {
        
        printf("An Error Has Occurred\n");
        return 1;
    }
    if (k >= rowSize) {
        printf("Invalid number of clusters!\n");
        return 1;
    }
    
    if( mainWork(k, iter,matrix) == 1){
        for (i = 0; i < rowSize; i++) {
            free(matrix[i]);
        }
        free(matrix);
        if(Row != NULL){
            free(Row);
        }
        printf("An Error Has Occurred");
        return 1;
    }
    for (i = 0; i < rowSize; i++) {
            free(matrix[i]);
        }
        free(matrix);
        if(Row != NULL){
            free(Row);
        }
        return 0;
}

int getK(int argc, char *argv[]) {
    int k = 0;
    char* k_str;
    int i = 0;
    if (argc < 2) {
        return -1;
    }
    k_str = argv[1];
    while (k_str[i] != '\0') {
        if (k_str[i] < '0' || k_str[i] > '9') {
            return -1;
        }
        k = k * 10 + (k_str[i] - '0');
        i++;
    }
    return k;
}

int getIter(int argc, char *argv[]) {
    int iter = 0;
    if (argc > 2) {
        char *iter_str = argv[2];
        int i = 0;
        while (iter_str[i] != '\0') {
            if (iter_str[i] < '0' || iter_str[i] > '9') {
                return DEFAULT_ITER;
            }
            iter = iter * 10 + (iter_str[i] - '0');
            i++;
        }
    }
    if (iter == 0) {
        return DEFAULT_ITER;
    }
    return iter;
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

int getVector() {
    char* line;
    size_t bufsize = 0;
    char* start;
    char* end;
    int index;
    if(getline(&line, &bufsize, stdin) == -1) {
        if(Row!=NULL)
        {
            free(Row);
            Row =NULL;
        }
        free(line);
        return 0;
    }
    if (vectorSize == 0) {
        vectorSize = countCommas(line) + 1;
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

int getMatrix() {
    int initialCapacity = 10;
    int capacity = initialCapacity;
    double* row;
    int flag;
    int i;
    double** resizedMatrix = NULL;
    matrix = (double**)malloc(initialCapacity * sizeof(double*));
    if (matrix == NULL) {
        return 1;
    }
    
    flag = getVector();
    if(flag == 1){
        for (i = 0; i < rowSize; i++) {
            free(matrix[i]);
        }
        free(matrix);
        return 1;
    }
    
    while (Row != NULL) {
        if (rowSize == capacity) {
            capacity *= 2;
            resizedMatrix = (double**)realloc(matrix, capacity * sizeof(double*));
            if (resizedMatrix == NULL) {
                for (i = 0; i < rowSize; i++) {
                    free(matrix[i]);
                }
                free(matrix);
                return 1;
            }
            matrix = resizedMatrix;
        }
        row = copyRow();
        if(row == NULL)
        {
            for (i = 0; i < rowSize; i++) {
                free(matrix[i]);
            }
            free(matrix);
            if(Row!=NULL)
            {
                free(Row);
                Row =NULL;
            }
            return 1;
        }
        matrix[rowSize] = row;
        rowSize++;
        flag = getVector(); 

        if(flag == 1){
            for (i = 0; i < rowSize; i++) {
                free(matrix[i]);
            }
            free(matrix);
            if(Row!=NULL)
            {
                free(Row);
                Row =NULL;
            }
            return 1;
        }

    }
    resizedMatrix = (double**)realloc(matrix, rowSize * sizeof(double*));
    if (resizedMatrix == NULL) {
        printf("An Error Has Occurred here\n");
        return 1;
    }
    
    matrix = resizedMatrix;
    return 0;
}

double* copyRow()
{
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

double** initialize(int k, double** AllVectors) {
    int i,j;
    double** centroids = malloc(k * sizeof(double*)); 
    if(centroids == NULL){
        return NULL;
    }
    for (i = 0; i < k; i++) {
        centroids[i] = malloc(vectorSize * sizeof(double));
        if(centroids[i]==NULL){
            for(j=0;j<i;j++){
                free(centroids[j]);
            }
            free(centroids);
            return NULL;
        }
    }
    for (i = 0; i < k; i++) {
        for (j = 0; j < vectorSize; j++) {
            centroids[i][j] = AllVectors[i][j];
        }
    }
    return centroids;
}


int printCentroids(double** centroids, int k,int len)
{
    int i,j;
    for(i=0;i<k;i++)
    {

        for(j=0;j<len;j++)
        {
            if(j!=0)
            {
                printf(",");
            }
            printf("%.4f",centroids[i][j]);
        }
        printf("\n");
    }
    return 0;
}


int k_means(double** AllVectors, int n, int len, int k, int max_iterations, double** centroids) {
    int i,j,l;
    int done = 0;
    i=0;
    while (i < max_iterations && done!=1) {
        double** old_centroids = malloc(k * sizeof(double*));
        int* cluster_sizes;
        double** clusters;
        done = 1;
        if(old_centroids == NULL){
            return 1;
        }
        cluster_sizes = calloc(k, sizeof(int));
        if(cluster_sizes == NULL){
            free(old_centroids);
            return 1;
        }
        clusters = malloc(k * sizeof(double*));
        if(clusters == NULL){
            free(old_centroids);
            free(cluster_sizes);
            return 1;
        }
        done = 1;
        for (j = 0; j < k; j++) {
            clusters[j] = calloc(len, sizeof(double));
            if(clusters[j] == NULL){
                for(l=0;l<j;l++)
                {
                    free(clusters[l]);
                    free(old_centroids[l]);
                }
                free(old_centroids);
                free(cluster_sizes);
                free(clusters);
                return 1;
            }
            old_centroids[j] = calloc(len, sizeof(double));
            if(old_centroids[j] == NULL){
                for(l=0;l<j;l++)
                {
                    free(clusters[l]);
                    free(old_centroids[l]);
                }
                free(clusters[j]);
                free(old_centroids);
                free(cluster_sizes);
                free(clusters);
                return 1;
            }
            for(l=0;l<len;l++){
                old_centroids[j][l] = centroids[j][l];
            }
        }
        for (j = 0; j < n; j++) {
            double min_distance = distance(AllVectors[j], centroids[0], len);
            int closest = 0;
            for (l = 0; l < k; l++) {
                double d = distance(AllVectors[j], centroids[l], len);
                if (d < min_distance) {
                    min_distance = d;
                    closest = l;
                }
            }
            for (l = 0; l < len; l++) {
                clusters[closest][l] += AllVectors[j][l];
            }
            cluster_sizes[closest]++;
        }
        
        for (j = 0; j < k; j++) {
            if (cluster_sizes[j] > 0) {
                for (l = 0; l < len; l++) {
                    centroids[j][l] = clusters[j][l] / cluster_sizes[j];
                }
            }
            free(clusters[j]);
        }
        for (j = 0; j < k; j++) {
            if (distance( old_centroids[j], centroids[j],len)>EPSILON) {
                done = 0;
            }
            free(old_centroids[j]);
        }
        free(old_centroids);
        free(clusters);
        free(cluster_sizes);
        i++;
    }
    return 0;
}

int mainWork(int k, int iter, double** AllVectors)
{
    int n = rowSize; 
    double** clusters = initialize(k, AllVectors);
    iter = iter;
    n=n;
    if(clusters == NULL)
    {
        return 1;
    }
    if( k_means(AllVectors, n,vectorSize, k, iter, clusters) == 1)
    {
        
        free(clusters);
        return 1;
    }
    if( printCentroids(clusters,k,vectorSize) == 1)
    {
        
        free(clusters);
        return 1;
    }
    
    free(clusters);
    return 0;
}

void printDoubleMatrix(int row) {
    int i,j;
    for (i = 0; i < row; i++) {
        for (j = 0; j < vectorSize; j++) {
            printf("%f", matrix[i][j]);
            printf(",");
        }
        printf("\n");
    }
}
