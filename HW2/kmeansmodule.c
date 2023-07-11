#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// Function declarations
double* getData(double* data);
static PyObject* fit(PyObject* self, PyObject* args);
int freeData(double** matrix, int rowSize);
double distance(double* a, double* b, int len);
int printCentroids(double** centroids, int k, int len);
int k_means(double** AllVectors, int n, int len, int k, int max_iterations, double** centroids, double epsilon);
int mainWork(int k, int iter, double** AllVectors, int rowSize, int vectorSize, double** centroids, double epsilon);
void printDoubleMatrix(double** matrix, int rownumber, int colnumber);

int main(int argc, char* argv[]) {
    return 0;
}

double* getData(double* data) {
    int i, j;
    int iter;
    int k;
    int dim, n;
    double** matrix = NULL;
    double** centroids = NULL;
    double epsilon;
    double* final = NULL;

    iter = (int)data[0];
    k = (int)data[1];
    n = (int)data[2];
    dim = (int)data[3];
    epsilon = (double)data[4];

    matrix = malloc(n * sizeof(double*));
    for (i = 0; i < n; i++) {
        matrix[i] = malloc(dim * sizeof(double));
        for (j = 0; j < dim; j++) {
            matrix[i][j] = data[5 + dim * i + j];
        }
    }

    centroids = malloc(k * sizeof(double*));
    for (i = 0; i < k; i++) {
        centroids[i] = malloc(dim * sizeof(double));
        for (j = 0; j < dim; j++) {
            centroids[i][j] = data[5 + dim * n + dim * i + j];
        }
    }
    if (mainWork(k, iter, matrix, n, dim, centroids, epsilon) != 0) {
        return NULL;
    }
    final = malloc((dim * k) * sizeof(double));
    for (i = 0; i < k; i++) {
        for (j = 0; j < dim; j++) {
            final[i * dim + j] = centroids[i][j];
        }
    }

    // Free dynamically allocated memory
    freeData(matrix, n);
    freeData(centroids, k);

    return final;
}

static PyObject* fit(PyObject* self, PyObject* args) {
    PyObject* lst;
    PyObject* item;
    PyObject* pyList;
    double num;
    double* res = NULL;
    if (!PyArg_ParseTuple(args, "O", &lst)) {
        return NULL;
    }

    int n = PyObject_Length(lst);
    if (n < 0) {
        return NULL;
    }

    double* nums = (double*)malloc(n * sizeof(double));
    if (nums == NULL) {
        printf("Memory allocation failed. Exiting.\n");
        return NULL;
    }
    int i;
    for (i = 0; i < n; i++) {
        item = PyList_GetItem(lst, i);
        num = PyFloat_AsDouble(item);
        nums[i] = num;
    }
    res = getData(nums);
    if (res == NULL) {
        free(nums);
        return NULL;
    }
    pyList = PyList_New(nums[1] * nums[3]);
    if (pyList == NULL) {
        // Error handling
        free(nums);
        return NULL;
    }

    for (i = 0; i < nums[1] * nums[3]; ++i) {
        PyObject* pyItem = PyFloat_FromDouble(res[i]);
        if (pyItem == NULL) {
            // Error handling
            Py_DECREF(pyList);
            free(nums);
            return NULL;
        }

        PyList_SET_ITEM(pyList, i, pyItem);
    }

    // Free dynamically allocated memory
    free(nums);
    free(res);

    return pyList;
}

static PyMethodDef k_meansMethods[] = {
    {
        "fit", // name exposed to Python
        (PyCFunction)fit, // C wrapper function
        METH_VARARGS, // received variable args (but really just 1)
        PyDoc_STR("Doing k_means main algorithm. Arguments: this function takes one Python list `lst` as input:\n"
        "- lst[0]: iter argument (as float and recast later)\n"
        "- lst[1]: k argument (as float and recast later)\n"
        "- lst[2]: n argument, the number of vectors (as float and recast later)\n"
        "- lst[3]: dim argument, the size of each vector (as float and recast later)\n"
        "- lst[4]: index argument\n"
        "- From this index onward, put the vectors information as follows:\n"
        "  Let `Vectors` be a 2D array containing the vectors where each line is a vector and each column in that line is a coordinate.\n"
        "  lst[5 + dim * i + j] will store the value of `Vectors[i][j]`.\n"
        "- From this index onward, put the centroids information as follows:\n"
        "  Let `Centroids` be a 2D array containing the centroids where each line is a centroid vector and each column in that line is a coordinate.\n"
        "  lst[5 + dim * n + dim * i + j] will store the value of `Centroids[i][j]`.\n"
        "Output: The output will be a Python list `final_List` containing the final centroid information:\n"
        "- The final centroids information as returned follows:\n"
        "  Let `finalCentroids` be a 2D array containing the final centroids where each line is a centroid vector and each column in that line is a coordinate.\n"
        "  `final_lst[dim * i + j]` will store the value of `finalCentroids[i][j]`.\n")
    },
    { NULL, NULL, 0, NULL } // Terminate the array
};

// modules definition
static struct PyModuleDef K_means_Module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",     // name of module exposed to Python
    NULL,              // module documentation
    -1,
    k_meansMethods
};

PyMODINIT_FUNC PyInit_mykmeanssp(void) {
    PyObject* m;
    m = PyModule_Create(&K_means_Module);
    if (!m) {
        return NULL;
    }
    return m;
}

int freeData(double** matrix, int rowSize) {
    if (matrix != NULL) {
        for (int i = 0; i < rowSize; i++) {
            free(matrix[i]);
        }
        free(matrix);
    }
    return 0;
}

double distance(double* a, double* b, int len) {
    double sum = 0;
    for (int i = 0; i < len; i++) {
        sum += pow(a[i] - b[i], 2);
    }
    return sqrt(sum);
}

int printCentroids(double** centroids, int k, int len) {
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < len; j++) {
            if (j != 0) {
                printf(",");
            }
            printf("%.4f", centroids[i][j]);
        }
        printf("\n");
    }
    return 0;
}

int k_means(double** AllVectors, int n, int len, int k, int max_iterations, double** centroids, double epsilon) {
    int i, j, l;
    int done = 0;
    i = 0;
    while (i < max_iterations && done != 1) {
        double** old_centroids = malloc(k * sizeof(double*));
        int* cluster_sizes;
        double** clusters;

        done = 1;
        if (old_centroids == NULL) {
            return 1;
        }
        cluster_sizes = calloc(k, sizeof(int));
        if (cluster_sizes == NULL) {
            free(old_centroids);
            return 1;
        }
        clusters = malloc(k * sizeof(double*));
        if (clusters == NULL) {
            free(old_centroids);
            free(cluster_sizes);
            return 1;
        }
        done = 1;
        for (j = 0; j < k; j++) {
            clusters[j] = calloc(len, sizeof(double));
            if (clusters[j] == NULL) {
                for (l = 0; l < j; l++) {
                    free(clusters[l]);
                    free(old_centroids[l]);
                }
                free(old_centroids);
                free(cluster_sizes);
                free(clusters);
                return 1;
            }
            old_centroids[j] = calloc(len, sizeof(double));
            if (old_centroids[j] == NULL) {
                for (l = 0; l < j; l++) {
                    free(clusters[l]);
                    free(old_centroids[l]);
                }
                free(clusters[j]);
                free(old_centroids);
                free(cluster_sizes);
                free(clusters);
                return 1;
            }
            for (l = 0; l < len; l++) {
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
            if (distance(old_centroids[j], centroids[j], len) > epsilon) {
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

int mainWork(int k, int iter, double** AllVectors, int rowSize, int vectorSize, double** centroids, double epsilon) {
    if (k_means(AllVectors, rowSize, vectorSize, k, iter, centroids, epsilon) == 1) {
        freeData(centroids, k);
        return 1;
    }

    //printCentroids(centroids, k, vectorSize);
    //freeData(centroids, k);
    return 0;
}

void printDoubleMatrix(double** matrix, int rownumber, int colnumber) {
    int i, j;
    for (i = 0; i < rownumber; i++) {
        for (j = 0; j < colnumber; j++) {
            printf("%f", matrix[i][j]);
            printf(",");
        }
        printf("\n");
    }
}
