import math
import numpy
import numpy as np
import mykmeanssp as Cfunc
import pandas as pd
import sys

np.random.seed(0)

def distance(a, b):
    return math.sqrt(sum([(a[i] - b[i]) ** 2 for i in range(len(a))]))

def initialize_centroids(k, vectors,vectorIndexes):
    np.random.seed(0)
    centroids = []
    n = len(vectors)
    probabilities = np.zeros(n)
    d_values = np.zeros(n)
    indexes = []
    chosen = []
    for i in range(len(vectors)):
        indexes.append(i)
    index = np.random.choice(indexes)
    chosen.append(int(vectorIndexes[index]))
    centroids.append(vectors[index].copy())
    while(len(centroids)<k):
        sum_d = 0
        for i in range(n):
            min_d = distance(centroids[0],vectors[i])
            for j in range(len(centroids)):
                curr_d = distance(centroids[j], vectors[i])
                if curr_d<min_d:
                    min_d = curr_d
            d_values[i] = min_d
            sum_d+=min_d
        for i in range(n):
            probabilities[i] = d_values[i]/sum_d
        index = np.random.choice(indexes, size=1, replace=True, p=probabilities)[0]
        chosen.append(int(vectorIndexes[index]))
        centroids.append(vectors[index].copy())
    return centroids,chosen

def handle_Files(fileName1, fileName2):
    file1 = pd.read_csv(fileName1, header = None)
    file2 = pd.read_csv(fileName2, header = None)
    final_file = pd.merge(file1,file2, how = 'inner' , on = file1.columns[0]) #################### find a way to merge based on the first column
    final_file = final_file.sort_values(final_file.columns[0], ascending=True)
    dim = final_file.shape[1]
    n= final_file.shape[0]
    headers = []
    for i in range(dim):
        headers.append(i)
    final_file.columns = headers
    rows = []
    for i in range(n):
        rows.append(i)
    final_file.index = rows
    vectorIndexes = []
    Vectors = np.empty((n,dim-1))
    for i in range(n):
        vectorIndexes.append(final_file[0][i])
        for j in range(1,dim): #################### not shure if it should be j or j+1 (because i dont know if the key column counts)
            Vectors[i][j-1] = final_file[j][i] ######################## not shure if it should be j or j+1 (because i dont know if the key column counts) 
    return Vectors,vectorIndexes

def printClusters(clusters):
    for i in range(len(clusters)):
        line = ""
        for j in range(len(clusters[i])):
            line+=str('%.4f' % clusters[i][j])+","
        print(line[:-1])



def mainWork(k, iter,epsilon, fileName1,fileName2):
    vectors,vectorIndexes = handle_Files(fileName1, fileName2)
    
    ###### chack if vectors is empty
    n = len(vectors)
    dim = len(vectors[0])
    if k<=1 or k>=n:
        return 1
    if iter<=1 or iter>=1000:
        return 2
    
    centroids , initial_Centroids = initialize_centroids(k, vectors,vectorIndexes)
    
    lst = continuos_List(iter,epsilon,k,dim,n,vectors,centroids)
    ###### call the c function here and send it lst
    final = Cfunc.fit(lst)
    ## call extract final
    res = extract_final(final,k,dim)
    printFirstChoise(initial_Centroids)
    printClusters(res)
    return 0

def main():
    try:
        vars = sys.argv[1:]
        k = int(vars[0])
        if len(vars)<5:
            iter = 300
            epsilon = float(vars[1])
            fileName1 = vars[2]
            fileName2 = vars[3]
        else:
            iter = int(vars[1])
            epsilon = float(vars[2])
            fileName1 = vars[3]
            fileName2 = vars[4]
        er = mainWork(k, iter,epsilon, fileName1,fileName2)
        if(er ==1):
            print("Invalid number of clusters!")
        if(er ==2):
            print("Invalid maximum iteration!")
    except:
        print("An Error Has Occurred")

def continuos_List(iter,EPSILON,k,dim,n,Vectors,centroids):
    lst = []
    lst.append(float(iter))
    lst.append(float(k))
    lst.append(float(n))
    lst.append(float(dim))
    lst.append(EPSILON)
    for i in range(n):
        for j in range(dim):
            lst.append(Vectors[i][j])
    for i in range(k):
        for j in range(dim):
            lst.append(centroids[i][j])
    return lst

def extract_final(lst,k,dim):
    res = []
    for i in range(k):
        res.append([])
        for j in range(dim):
            res[i].append(lst[i*dim+j])
    return res

def printFirstChoise(chosen):
    
    res = ""
    for i in range(len(chosen)):
        res+=str(chosen[i])+","
    print(res[:-1])


main()