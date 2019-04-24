import sys
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.legend
from subprocess import call
from timeit import default_timer as timer


name0=sys.argv[1]
name1=sys.argv[2]
n=int(input("Enter number of randoms file: "))

print("Starting DDD...")
t0=timer()
call(["./XXX", name0, "DDD"])
print(round(timer()-t0,3),'seconds')

for i in range(n):
    t=timer()
    print("Startig RRR"+str(i))
    call(["./XXX", name1+np.str(i)+".dat", "RRR"+np.str(i)])
    print(round(timer()-t,3),' seconds')
    t=timer()
    print("Startig DDR"+str(i))
    call(["./XXY", name0, name1+np.str(i)+".dat", "DRR"+np.str(i)])
    print(round(timer()-t,3),' seconds')
    t=timer()
    print("Startig DRR"+str(i))
    call(["./XXY", name1+np.str(i)+".dat", name0, "DDR"+np.str(i)])
    print(round(timer()-t,3),' seconds')

DDD=np.loadtxt('DDD.dat')

total_hist=[]
total_hist1=[]

for j in range(n):
    RRR=np.loadtxt('RRR'+str(j)+'.dat')
    DDR=np.loadtxt('DDR'+str(j)+'.dat')
    DRR=np.loadtxt('DRR'+str(j)+'.dat')
    N_D=int(DDD[len(DDD)-1][0])
    N_R=int(RRR[len(RRR)-1][0])
    hist=[]
    hist1=[]
    for i in range(len(DDD)-1):
        hist.append([DDD[i][0],(DDD[i][1]/(N_D**3/6)-3*DDR[i][1]/(N_D**2*N_R/2)+3*DRR[i][1]/(N_D*N_R**2/2)-RRR[i][1]/(N_R**3/6))/(RRR[i][1]/(N_R**3/6)),(DDD[i
                                                                                                                                                             ][2]/(N_D**3/6)-3*DDR[i][2]/(N_D**2*N_R/2)+3*DRR[i][2]/(N_D*N_R**2/2)-RRR[i][2]/(N_R**3/6))/(RRR[i][2]/(N_R**3/6)),(DDD[i][3]/(N_D**3/6)-3*DDR[i][3]/(N_D**2*N
                                                                                                                                                                                                                                                                                                                   _R/2)+3*DRR[i][3]/(N_D*N_R**2/2)-RRR[i][3]/(N_R**3/6))/(RRR[i][3]/(N_R**3/6))])
        hist1.append([DDD[i][0],(DDD[i][1]/(N_D**3/6))/(RRR[i][1]/(N_R**3/6))-1])
    total_hist.append(hist)
    total_hist1.append(hist1)

hist=total_hist[0]
hist1=total_hist1[0]
for i in range(1,n):
    for j in range(len(total_hist[0])):
        for k in range(4):
            hist[j][k]=hist[j][k]+total_hist[i][j][k]
        for k in range(2):
            hist1[j][k]=hist1[j][k]+total_hist1[i][j][k]


for i in range(len(hist)):
    for j in range(4):
        hist[i][j]=hist[i][j]/n
    for j in range(2):
        hist1[i][j]=hist1[i][j]/n


hist=np.array(hist)    
np.savetxt('hist.dat',hist)


hist1=np.array(hist1)
np.savetxt('hist1.dat',hist1)

print(round((timer()-t0)/60,3),'minutes')
