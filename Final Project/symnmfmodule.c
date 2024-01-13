#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "matrix.h"



static PyObject* symnmf(PyObject* self, PyObject* args);
static PyObject* sym(PyObject* self, PyObject* args);
static PyObject* ddg(PyObject* self, PyObject* args);
static PyObject* norm(PyObject* self, PyObject* args);
double* getSymnmfData(double* data);
double* getSymData(double* data);
double* getDdgData(double* data);
double* getNormData(double* data);


static PyMethodDef symnmf_Methods[] = {
    {
        "symnmf",
        (PyCFunction)symnmf, 
        METH_VARARGS, 
        PyDoc_STR("")
    },
    {
        "sym", 
        (PyCFunction)sym, 
        METH_VARARGS, 
        PyDoc_STR("")
    },
    {
        "ddg", 
        (PyCFunction)ddg, 
        METH_VARARGS, 
        PyDoc_STR("")
    },
    {
        "norm", 
        (PyCFunction)norm, 
        METH_VARARGS, 
        PyDoc_STR("")
    },
    { NULL, NULL, 0, NULL } 
};

static struct PyModuleDef mysymnmf_Module = {
    PyModuleDef_HEAD_INIT,
    "mysymnmf",     
    NULL,              
    -1,
    symnmf_Methods
};

PyMODINIT_FUNC PyInit_mysymnmf(void) {

    PyObject* m;
    m = PyModule_Create(&mysymnmf_Module);
    if (!m) {
        return NULL;
    }
    return m;
}


static PyObject* symnmf(PyObject* self, PyObject* args) {
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
    res = getSymnmfData(nums);
    if (res == NULL) {
        free(nums);
        return NULL;
    }
    pyList = PyList_New(nums[2] * nums[1]);
    if (pyList == NULL) {
        free(nums);
        free(res);
        return NULL;
    }

    for (i = 0; i < nums[2] * nums[1]; ++i) {
        PyObject* pyItem = PyFloat_FromDouble(res[i]);
        if (pyItem == NULL) {
            Py_DECREF(pyList);
            free(nums);
            free(res);
            return NULL;
        }

        PyList_SET_ITEM(pyList, i, pyItem);
    }

    free(nums);
    free(res);

    return pyList;
}

double* getSymnmfData(double* data) {
    int i, j;
    int k;
    int dim, n;
    int epsilon_check =0;
    int max_iter =300;
    Matrix* WMatrix;
    Matrix* HMatrix;
    Matrix* newHMatrix;
    Matrix* diffHMatrix;
    double epsilon,diff;
    double* final = NULL;
    k = (int)data[1];
    n = (int)data[2];
    dim = (int)data[3];
    epsilon = (double)data[4];
    dim+=1;
    dim-=1;
    WMatrix = createMatrix(n,n);
    if(WMatrix==NULL){
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            WMatrix->data[i][j] = data[5 + n * i + j];
        }
    }
    HMatrix = createMatrix(n,k);
    if(HMatrix==NULL){
        freeMatrix(WMatrix);
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < k; j++) {
            HMatrix->data[i][j] = data[5 + n * n + k * i + j];
        }
    }
    i=0;
    while (epsilon_check==0 && i<max_iter)
    {
        newHMatrix = UpdatematrixH(WMatrix, HMatrix);
        if(newHMatrix==NULL){
            freeMatrix(WMatrix);
            freeMatrix(HMatrix);
            return NULL;
        }
        diffHMatrix = SubtractionMatrices(newHMatrix,HMatrix);
        if(diffHMatrix==NULL){
            freeMatrix(WMatrix);
            freeMatrix(HMatrix);
            freeMatrix(newHMatrix);
            return NULL;
        }
        diff = FrobeniusnormSquared(diffHMatrix);
        if (diff<epsilon){
            epsilon_check = 1;
        }
        freeMatrix(HMatrix);
        freeMatrix(diffHMatrix);
        HMatrix = newHMatrix;
        i++;
    }
    final = malloc((n*k) * sizeof(double));//error 
    if(final == NULL){
        freeMatrix(HMatrix);
        freeMatrix(WMatrix);
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < k; j++) {
            final[k*i+j] = HMatrix->data[i][j];
        }
    }
    freeMatrix(HMatrix);
    freeMatrix(WMatrix);
    return final;
}

static PyObject* sym(PyObject* self, PyObject* args) {
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
    res = getSymData(nums);
    if (res == NULL) {
        free(nums);
        return NULL;
    }
    pyList = PyList_New(nums[2] * nums[2]);
    if (pyList == NULL) {
        free(nums);
        free(res);
        return NULL;
    }
    for (i = 0; i < nums[2] * nums[2]; ++i) {

        PyObject* pyItem = PyFloat_FromDouble(res[i]);
        if (pyItem == NULL) {

            Py_DECREF(pyList);
            free(nums);
            free(res);
            return NULL;
        }

        PyList_SET_ITEM(pyList, i, pyItem);
    }
    free(nums);
    free(res);

    return pyList;
}

double* getSymData(double* data) {
    int i, j;
    int dim, n;
    Matrix* XMatrix;
    Matrix* AMatrix;
    double* final = NULL;
    n = (int)data[2];
    dim = (int)data[3];
    XMatrix = createMatrix(n, dim);
    if(XMatrix == NULL){
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < dim; j++) {
            XMatrix->data[i][j] = data[5 + dim * i + j];
        }
    }
    AMatrix =calculateSimilarityMatrix(XMatrix);
    if(AMatrix == NULL){
        freeMatrix(XMatrix);
        return NULL;
    }
    final = malloc((n*n) * sizeof(double));
    if(final == NULL){
        freeMatrix(XMatrix);
        freeMatrix(AMatrix);
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            final[n*i+j] = AMatrix->data[i][j];
        }
    }
    freeMatrix(XMatrix);
    freeMatrix(AMatrix);
    return final;
}

static PyObject* ddg(PyObject* self, PyObject* args) {
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
    res = getDdgData(nums);
    if (res == NULL) {
        free(nums);
        return NULL;
    }
    pyList = PyList_New(nums[2] * nums[2]);
    if (pyList == NULL) {
        free(nums);
        free(res);
        return NULL;
    }
    for (i = 0; i < nums[2] * nums[2]; ++i) {
        PyObject* pyItem = PyFloat_FromDouble(res[i]);
        if (pyItem == NULL) {
            Py_DECREF(pyList);
            free(nums);
            free(res);
            return NULL;
        }
        PyList_SET_ITEM(pyList, i, pyItem);
    }
    free(nums);
    free(res);
    return pyList;
}

double* getDdgData(double* data) {
    int i, j;
    int dim, n;
    Matrix* XMatrix;
    Matrix* AMatrix;
    Matrix* DMatrix;
    double* final = NULL;
    n = (int)data[2];
    dim = (int)data[3];
    XMatrix = createMatrix(n, dim);
    if(XMatrix==NULL){
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < dim; j++) {
            XMatrix->data[i][j] = data[5 + dim * i + j];
        }
    }
    AMatrix = calculateSimilarityMatrix(XMatrix);
    if(AMatrix==NULL){
        freeMatrix(XMatrix);
        return NULL;
    }
    DMatrix = calculateDiagonalDegreeMatrix(AMatrix);
    if(DMatrix == NULL){
        freeMatrix(XMatrix);
        freeMatrix(AMatrix);
        return NULL;
    }
    final = malloc((n*n) * sizeof(double));
    if(final == NULL){
        freeMatrix(XMatrix);
        freeMatrix(AMatrix);
        freeMatrix(DMatrix);
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            final[n*i+j] = DMatrix->data[i][j];
        }
    }
    freeMatrix(XMatrix);
    freeMatrix(AMatrix);
    freeMatrix(DMatrix);
    return final;
}


static PyObject* norm(PyObject* self, PyObject* args) {
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
    res = getNormData(nums);
    if (res == NULL) {
        free(nums);
        return NULL;
    }
    pyList = PyList_New(nums[2] * nums[2]);
    if (pyList == NULL) {
        free(nums);
        free(res);
        return NULL;
    }
    for (i = 0; i < nums[2] * nums[2]; ++i) {
        PyObject* pyItem = PyFloat_FromDouble(res[i]);
        if (pyItem == NULL) {
            Py_DECREF(pyList);
            free(nums);
            free(res);
            return NULL;
        }
        PyList_SET_ITEM(pyList, i, pyItem);
    }
    free(nums);
    free(res);
    return pyList;
}

double* getNormData(double* data) {
    int i, j;
    int dim, n;
    Matrix* XMatrix;
    Matrix* WMatrix;
    double* final = NULL;
    n = (int)data[2];
    dim = (int)data[3];
    XMatrix = createMatrix(n, dim);
    if(XMatrix == NULL){
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < dim; j++) {
            XMatrix->data[i][j] = data[5 + dim * i + j];
        }
    }
    WMatrix = calculateNormalizedSimilarityMatrix(XMatrix);
    if(WMatrix == NULL){
        freeMatrix(XMatrix);
        return NULL;
    }
    final = malloc((n*n) * sizeof(double));
    if(final == NULL){
        freeMatrix(XMatrix);
        freeMatrix(WMatrix);
        return NULL;
    }
    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            final[n*i+j] = WMatrix->data[i][j];
        }
    }
    freeMatrix(XMatrix);
    freeMatrix(WMatrix);
    return final;
}