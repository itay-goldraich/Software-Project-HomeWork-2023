import math
import numpy
import numpy as np
import mysymnmf as Cfunc
import pandas as pd
import sys

np.random.seed(0)

def distance(a, b):
    return math.sqrt(sum([(a[i] - b[i]) ** 2 for i in range(len(a))]))

def main():
    try:
        vars = sys.argv[1:]
        k = int(vars[0])
        goal = vars[1]
        fileName = vars[2]
        vectors,vectorIndexes = getVectors(fileName)
        res = None
        if goal == "symnmf":
            res = symnmfGoal(k,vectors,vectorIndexes)
        elif goal == "sym":
            res = symGoal(k,vectors,vectorIndexes)
        elif goal == "ddg":
            res = ddgGoal(k,vectors,vectorIndexes)
        elif goal == "norm":
            res = normGoal(k,vectors,vectorIndexes)
        printMatrix(res)
    except:
        print("An Error Has Occurred")

def printMatrix(mat):
    for i in range(len(mat)):
        line = ""
        for j in range(len(mat[i])):
            line+=str('%.4f' % mat[i][j])
            if(j<=len(mat[i])-2):
                line+=","
        print(line)


def getVectors(fileName):
    f = pd.read_csv(fileName, header = None)
    dim = f.shape[1]
    n= f.shape[0]
    
    vectorIndexes = []
    Vectors = np.empty((n,dim))
    for i in range(n):
        vectorIndexes.append(f[0][i])
        for j in range(0,dim): 
            Vectors[i][j] = f[j][i]
    return Vectors,vectorIndexes


def symnmfGoal(k,vectors,vectorIndexes):
    n = len(vectors)
    dim = len(vectors[0])
    WMatrix = normGoal(k,vectors,vectorIndexes)
    W_Avg = Matrix_avg(WMatrix)
    HMatrix = np.random.uniform(0, 2*math.sqrt(W_Avg/k), size=(n ,k))

    lst = symnmf_continuos_List(1e-4,k,dim,n,WMatrix,HMatrix)
    final = Cfunc.symnmf(lst)
    res = extractFinalMatrix(final,n,k)
    return res

def extractFinalMatrix(final,row,col):
    res = np.zeros((row, col))
    for i in range(row):
        for j in range(col):
            res[i][j] = final[i*col+j]
    return res

def Matrix_avg(mat):
    sum = 0
    res = 0
    n = len(mat)
    if n>0:
        dim = len(mat[0])
        for i in range(n):
            for j in range(dim):
                sum+=mat[i][j]
        res = sum/(n*dim)
    return res

def symnmf_continuos_List(EPSILON,k,dim,n,WMatrix,HMatrix):
    lst = []
    lst.append(0)
    lst.append(float(k))
    lst.append(float(n))
    lst.append(float(dim))
    lst.append(EPSILON)
    for i in range(n):
        for j in range(n):
            lst.append(WMatrix[i][j])
    for i in range(n):
        for j in range(k):
            lst.append(HMatrix[i][j])
    return lst

def symGoal(k,vectors,vectorIndexes):
    n = len(vectors)
    dim = len(vectors[0])
    lst = continuos_List(1,1e-4,k,dim,n,vectors)
    final = Cfunc.sym(lst)
    res = extractFinalMatrix(final,n,n)
    return res

def continuos_List(goal,EPSILON,k,dim,n,Vectors):
    lst = []
    lst.append(1)
    lst.append(float(k))
    lst.append(float(n))
    lst.append(float(dim))
    lst.append(EPSILON)
    for i in range(n):
        for j in range(dim):
            lst.append(Vectors[i][j])
    return lst


def ddgGoal(k,vectors,vectorIndexes):
    n = len(vectors)
    dim = len(vectors[0])
    lst = continuos_List(2,1e-4,k,dim,n,vectors)
    final = Cfunc.ddg(lst)
    res = extractFinalMatrix(final,n,n)
    return res


def normGoal(k,vectors,vectorIndexes):
    n = len(vectors)
    dim = len(vectors[0])
    lst = continuos_List(3,1e-4,k,dim,n,vectors)
    final = Cfunc.norm(lst)
    res = extractFinalMatrix(final,n,n)
    return res



main()