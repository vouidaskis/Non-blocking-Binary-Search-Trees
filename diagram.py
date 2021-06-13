import sys
import matplotlib
import numpy as np
from numpy import *
import math
import matplotlib.pyplot as plt


lines = [line.rstrip('\n') for line in open('data10000_20_10_5.txt')]
lines1 = [line1.rstrip('\n') for line1 in open('data10000_20_10_10.txt')]
lines2 = [line2.rstrip('\n') for line2 in open('data10000_20_10_20.txt')]
num = len(lines)  # synolo  deigmatwn
arr = [0 for i in range(num)]
arr1 = [0 for i in range(num)]
arr2 = [0 for i in range(num)]

j=0
x = [0 for i in range(num-1)]
y = [0 for i in range(num-1)]
x1 = [0 for i in range(num-1)]
y1 = [0 for i in range(num-1)]
x2 = [0 for i in range(num-1)]
y2 = [0 for i in range(num-1)]
for data in lines:
    arr[j] = data.split(',')
    j = j+1
j=0
for data1 in lines1:
    arr1[j] = data1.split(',')
    j = j+1
j=0
for data2 in lines2:
    arr2[j] = data2.split(',')
    j = j+1

for row in range(num):
    if(row != 0):
        x[row-1] = float(arr[row][1])
        y[row-1] = float(arr[row][17])

        y1[row - 1] = float(arr1[row][17])

        y2[row - 1] = float(arr2[row][17])




x = np.array(x)
y1 = np.array(y1)/100000
y2 = np.array(y2)/100000
y = np.array(y)/100000

plt.plot(x, y, '--o', label = "5 second")
plt.plot(x, y1, '--o', label = "10 second")
plt.plot(x, y2, '--o', label = "20 second")



plt.xlabel('number of threads')
# naming the y axis
plt.ylabel('throughput ')
# giving a title to my graph
plt.title('20 insert 10 delete 70 find and 10000 size')

# show a legend on the plot
plt.legend()
plt.savefig('20i_10_d_10000_size.png')
# function to show the plot
plt.show()




