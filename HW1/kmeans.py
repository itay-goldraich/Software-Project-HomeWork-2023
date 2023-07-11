import math
import sys

EPSILON = 0.001
#def distance(a, b, length):
#    sum = 0
#    for i in range(length):
 #       sum += ((a[i] - b[i]) ** 2)
#    return math.sqrt(sum)

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
    Vectors = []
    f = open(fileName)
    for line in f:
        vectorSt = line.split(',')
        vector = []
        for i in range(len(vectorSt)):
            vector.append(float(vectorSt[i]))
        Vectors.append(vector)
    f.close()
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

#################################################################################################

def distance(a, b):
    return math.sqrt(sum([(a[i] - b[i]) ** 2 for i in range(len(a))]))

def k_means(AllVectors, k,centroids, max_iterations):
    done = False
    i=0
    while i<max_iterations and not done:
        done = True
        oldCentroids = centroids.copy()
        # Assign each data point to its closest centroid
        clusters = [[] for j in range(k)]
        for point in AllVectors:
            distances = [distance(point, centroid) for centroid in centroids]
            closest = distances.index(min(distances))
            clusters[closest].append(point)
        # Update centroids to be the mean of the assigned data points
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

#################################################################################################


def mainWork(k,iter,fileName):
    AllVectors = getVectors(fileName)
    
    if k>=len(AllVectors) or k<=1 :
        print("Invalid number of clusters!")
    elif iter<=1 or iter>=1000:
        print("Invalid maximum iteration!")
    else:
        clusters = initialize(k, AllVectors)
        clusters = k_means(AllVectors, k,clusters, iter)
        #i=0
        
        #tooSmallDiff = False
        #while i<iter and not tooSmallDiff:
        #    old = AllVectors.copy()
        #    tooSmallDiff = singleIter(clusters, AllVectors)
        #    i+=1
        printClusters(clusters)

def main():
    try:
        Vars = sys.argv[1:]
        k = int(Vars[0])
        if len(Vars)<3:
            iter = 200
            fileName = Vars[1]
        else:
            iter = int(Vars[1])
            fileName = Vars[2]
        mainWork(k, iter, fileName)
    except:
        print("An Error Has Occurred")


main()