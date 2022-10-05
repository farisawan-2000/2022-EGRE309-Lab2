import sys, os
import matplotlib.pyplot as plt
import numpy as np


with open(sys.argv[1]) as f:
    fl = f.readlines()

    itercount = int(fl[0])
    itersize = int(fl[1])

    grid = np.zeros((itersize, itersize))

    for i in range(len(fl[2:])):
        data = fl[2 + i].split()
        for j in range(len(data)):
            grid[j][i] = float(data[j])


    plt.contourf(grid, levels=100)
    plt.colorbar()
    plt.title("test")
    plt.xticks(range(0,5),range(5,10))
    plt.show()
