import sys
import time
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

f = open("info.txt", "r")

MAXY = 100

current = 0

fig, ax = plt.subplots()
xdata, ydata = [], []
xdata2, ydata2 = [], []
xdata3, ydata3 = [], []


ln1, ln2, ln3, ln4 = ax.plot([], [], 'r-',
                         [], [], 'b-', 
                         [], [], 'y-',  
                         [], [], 'c-', 
                         animated=False) #animated is associated with blit
plt.setp(ln1, label='Ant')
plt.setp(ln2, label='Doodlebug')
plt.setp(ln3, label='Plant')
plt.legend(loc=0, numpoints=1)

                         
def init():
    ax.set_xlim(0, 500)
    ax.set_ylim(0, MAXY)    
    return ln1,ln2,ln3,ln4

def update(i): #i is an int from 0 to frames-1, and keep looping
    global MAXY
    global f
   
    filenum = f.tell()
    #print(filenum)

    global current
   
    xlen = len(open("info.txt",'r').readlines())
    wait=0
    while current >= xlen:
        wait = wait+1
        if wait > 10:
            sys.exit(0)
        time.sleep(1)
        xlen = len(open("info.txt",'r').readlines())

    line = f.readline()
    line = line.replace('\n','')
    str1 = line.split(' ')
    while len(str1) < 4 :
        f.close()
        f = open("info.txt", "r")
        f.seek(filenum)
        line = f.readline()
        line = line.replace('\n','')
        str1 = line.split(' ')
        time.sleep(1)

    x = int(str1[0])
    y1 = int(str1[1])
    y2 = int(str1[2])
    y3 = int(str1[3])
    
    current = x
    
    ax.set_xlim(x-490, 10+x)
    ax.set_ylim(0, MAXY)    
    
    #print(x, " ", y1) 
    xdata.append(x)
    ydata.append(y1)
    xdata2.append(x)
    ydata2.append(y2)
    xdata3.append(x)
    ydata3.append(y3)

    if x % 500 == 0:
        MAXY = y1

    if y1 > MAXY: 
        MAXY = y1
    if y2 > MAXY: 
        MAXY = y2
    if y3 > MAXY: 
        MAXY = y3

    ln1.set_data(xdata, ydata)
    ln2.set_data(xdata2, ydata2)
    ln3.set_data(xdata3, ydata3)
    return ln1, ln2, ln3, ln4

def main():
    ani = FuncAnimation(fig, update, frames=500, interval = 1,
                    init_func=init, blit=False)
#    ani.save('animation.mp4', writer='ffmpeg', fps=30) /save as .mp4
    plt.show() 
        
if __name__ == '__main__':
    main()  
