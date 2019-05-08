import os
import subprocess
import random
time=[[0 for x in range(8)]for y in range(9)]
a=[1,2,4,8,16,32,64,128,256]
b=[1,2,3,4,5,6,7,8]
import texttable
table = texttable.Texttable()
table.header(['x']+b)
for i in range(9):
    temp=[a[i]]
    for j in range(8):
        os.system("g++ gen.c -o gen")
        os.system("g++ QR.cpp -fopenmp -o QR")
        x="./gen " + str(a[i]) + " > input"
        os.system(x)
        y="./QR -n "+str(b[j]) + " -file input -silent"
        result=subprocess.Popen(y.split(),stdout=subprocess.PIPE)
        out,e=result.communicate()  
        temp.append(out)
        os.system("rm gen QR ")
        print i,j
        #temp.append(((60.0/256)*a[i])/b[j]+random.random()*random.choice([1,2,3,0])/10)
    table.add_row(temp)
print(table.draw())


