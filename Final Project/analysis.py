import math
import numpy
import numpy as np
import mysymnmf as Cfunc
import pandas as pd
import sys
from sklearn.metrics import silhouette_score

np.random.seed(0)

def main():
    try:
        Vars = sys.argv[1:]
        k = int(Vars[0])
        fileName = Vars[1]
        vectors = getVectors(fileName)
        if(len(vectors)>0):
            centroids = mainWork(k,300,fileName)
            HMatrix = symmnf(k,vectors)
            NPcentroids = matrixToNP(centroids)
            NPHMatrix = matrixToNP(HMatrix)
            HMatrixClusterList = calculateHMatrixClusters(HMatrix)
            KMeansClusterList = calculateKMeansClusters(centroids,vectors)
            symmnfSilhuetteScore = silhouette_score(vectors, HMatrixClusterList)
            KMeansSilhuetteScore = silhouette_score(vectors, KMeansClusterList)
            print("nmf: "+str('%.4f' % symmnfSilhuetteScore))
            print("kmeans: "+str('%.4f' % KMeansSilhuetteScore))
    except:
        print("An Error Has Occurred")

def calculateHMatrixClusters(HMatrix):
    res = np.zeros(len(HMatrix))
    for i in range(len(HMatrix)):
        maxVal = HMatrix[i][0]
        maxCluster = 0
        for j in range(len(HMatrix[i])):
            if HMatrix[i][j]>maxVal:
                maxVal = HMatrix[i][j]
                maxCluster = j
        res[i] = maxCluster
    return res

def calculateKMeansClusters(centroids,vectors):
    res = np.zeros(len(vectors))
    for i in range(len(vectors)):
        minDistance = distance(vectors[i],centroids[0])
        closestCentroid = 0
        for j in range(len(centroids)):
            dist = distance(vectors[i],centroids[j])
            if minDistance>dist:
                minDistance = dist
                closestCentroid = j
        res[i] = closestCentroid
    return res


def matrixToNP(mat):
    res = np.zeros((len(mat),len(mat[0])))
    for i in range(len(mat)):
        for j in range(len(mat[0])):
            res[i][j]=mat[i][j]
    return res

def symmnf(k,vectors):
    n = len(vectors)
    dim = len(vectors[0])
    WMatrix = normGoal(k,vectors)
    W_Avg = Matrix_avg(WMatrix)
    HMatrix = random_matrix = np.random.uniform(0, 2*math.sqrt(W_Avg/k), size=(n ,k))
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

def normGoal(k,vectors):
    n = len(vectors)
    dim = len(vectors[0])
    lst = continuos_List(3,1e-4,k,dim,n,vectors)
    final = Cfunc.norm(lst)
    res = extractFinalMatrix(final,n,n)
    return res


def printMatrix(mat):
    for i in range(len(mat)):
        line = ""
        for j in range(len(mat[i])):
            line+=str('%.4f' % mat[i][j])
            if(j<=len(mat[i])-2):
                line+=","
        print(line)


def printVector(mat):
    for i in range(len(mat)):
        line+=str('%.4f' % mat[i][j])+","
    print(line[:-1])

EPSILON = 0.001

def initialize(k,AllVectors):
    clusters = []
    for i in range(k):
        clusters.append([])
        for j in range(len(AllVectors[i])):
            clusters[i].append(AllVectors[i][j])
    return clusters

def closestCluster(clusters,vector):
    min = float('inf')
    length = len(vector)
    for i in range(len(clusters)):
        dist = distance(clusters[i], vector, length)
        if dist<min:
            min = dist
            closest = i
    return closest

def updateCentroidIsFinished(clusters,index,sumVectors,numVectors):
    new = []
    length = len(clusters[index])
    for i in range(len(clusters[index])):
        new.append(sumVectors[index][i]/numVectors[index])
    res = False
    if distance(clusters[index], new, length)<EPSILON:
        res = True
    clusters[index] = new
    return res

def getVectors(fileName):
    f = pd.read_csv(fileName, header = None)
    dim = f.shape[1]
    n= f.shape[0]
    
    Vectors = np.empty((n,dim))
    for i in range(n):
        for j in range(0,dim): 
            Vectors[i][j] = f[j][i] 
    return Vectors

def singleIter(clusters,AllVectors):
    numVectors = []
    sumVectors = []
    for i in range(len(clusters)):
        numVectors.append(float(0))
        sumVectors.append([])
        for j in range(len(clusters[i])):
            sumVectors[i].append(float(0))
    for i in range(len(AllVectors)):
        vector = AllVectors[i]
        closest = closestCluster(clusters, vector)
        numVectors[closest]+=1
        for j in range(len(clusters[closest])):
            sumVectors[closest][j]+=vector[j]
    res = True
    for i in range(len(clusters)):
        res = res and updateCentroidIsFinished(clusters,i,sumVectors,numVectors)
    return res

def printClusters(clusters):
    for i in range(len(clusters)):
        line = ""
        for j in range(len(clusters[i])):
            line+=str('%.4f' % clusters[i][j])+","
        print(line[:-1])



def distance(a, b):
    return math.sqrt(sum([(a[i] - b[i]) ** 2 for i in range(len(a))]))

def k_means(AllVectors, k,centroids, max_iterations):
    done = False
    i=0
    while i<max_iterations and not done:
        done = True
        oldCentroids = centroids.copy()
        clusters = [[] for j in range(k)]
        for point in AllVectors:
            distances = [distance(point, centroid) for centroid in centroids]
            closest = distances.index(min(distances))
            clusters[closest].append(point)
        for j in range(k):
            if clusters[j]:
                newVector = [0 for index in range(len(centroids[j]))]
                for index in range(len(centroids[j])):
                    for vec in clusters[j]:
                        newVector[index]+=vec[index]
                    newVector[index] = newVector[index]/len(clusters[j])
                centroids[j] = newVector
        for index in range(len(centroids)):
            done = done and (distance(oldCentroids[index], centroids[index])<EPSILON)
        i+=1
    return centroids

def mainWork(k,iter,fileName):
    AllVectors = getVectors(fileName)
    if k>=len(AllVectors) or k<=1 :
        print("Invalid number of clusters!")
    elif iter<=1 or iter>=1000:
        print("Invalid maximum iteration!")
    else:
        clusters = initialize(k, AllVectors)
        clusters = k_means(AllVectors, k,clusters, iter)
        return clusters


main()