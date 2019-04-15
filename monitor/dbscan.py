#kangtuanjie
#coding:utf-8
#数据的聚类处理
import numpy as np
from sklearn.cluster import DBSCAN
from sklearn import metrics

import matplotlib.pyplot as plt 
from mpl_toolkits.mplot3d import Axes3D
import numpy.matlib as mat # import the matrix 

import time
time1 = time.time()

X = []

#打开数据集文件
file = open("newData.txt","r")
for line in file.readlines():
    lineArr = line.strip().split(',')  
    X.append([float(lineArr[0]), float(lineArr[1]), float(lineArr[2])]) 
file.close()
X = np.mat(X)
#X = np.ndarray(X)
x,y = X.shape
#_________________________________________________________
EPS = 1.6
MINPTS = int(x/10)
db = DBSCAN(eps=EPS, min_samples=MINPTS).fit(X)
core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
core_samples_mask[db.core_sample_indices_] = True
labels = db.labels_
#_____________________________________________________________
numSamples,dim=X.shape
mark = ['r','b','g', 'y', 'c', 'm', 'k']

n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)
print("clusting num:",n_clusters_)
if n_clusters_ > len(mark):  
    print("Sorry! Your k is too large! please contact Zouxy")
 
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
# draw all samples 
for i in range(numSamples):
    if labels[i]!=-1:
        markIndex = int(labels[i])
    else:
        markIndex = int(-1)
    ax.scatter(X[i,0], X[i,1], X[i,2], c=mark[markIndex])
ax.set_xlabel('data1')
ax.set_ylabel('data2')
ax.set_zlabel('data3')


################### Result ###################
ratio = 0
mean = []
amount_clusting = []
static_result = [] 

ratio = len(labels[labels[:] == -1]) / len(labels)
for i in range(n_clusters_):
    
    print(i,":")
    amount_clusting.append(len(labels[labels[:] == i]))
    mean_of_cluster = [np.mean(X[labels==i][:,0]),np.mean(X[labels==i][:,1]),np.mean(X[labels==i][:,2])]
    mean.append(mean_of_cluster)
    print(mean_of_cluster)
# draw the centroids
for i in range(n_clusters_): 
    ax.scatter(mean[i][0], mean[i][1], mean[i][2], s=78, c=mark[-1], marker='^') #'k'



plt.savefig("pic.png", dpi=1000) # D:/codeshang/tainEnvMonitor1/monitor/

file = open("result.txt","w") #D:/codeshang/tainEnvMonitor1/monitor/
file.write("Eps="+str(EPS)+'\n')
file.write("MinPts="+str(MINPTS)+'\n')
file.write("噪声点数量："+str(len(labels[labels[:] == -1]))+"\n")
file.write("噪声比例："+str(round(ratio,5))+"\n") #
file.write("簇的总数为："+str(n_clusters_)+"\n\n") 
for cluster in range(n_clusters_):
    file.write("No."+str(cluster)+":"+"\n")
    file.write("  "+"数量："+str(amount_clusting[cluster])+"\n")
    file.write("  "+"中心点："+"("+str(round(mean[cluster][0],2))+","+str(round(mean[cluster][1],2))+","+str(round(mean[cluster][2],2))+")"+"\n")
    file.write("\n")

time2=time.time()
print('time:',time2-time1)
file.write("Dbscan算法用时："+str(round(time2-time1,2))+"s"+"\n")

file.close()



