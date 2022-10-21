import sys, os
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from math import sqrt

with open(sys.argv[1]) as f:
    fl = f.readlines()

    itercount = int(fl[0])
    itersize = int(fl[1])

    grid = np.zeros((itersize, itersize))

    for i in range(len(fl[2:])):
        data = fl[2 + i].split()
        for j in range(len(data)):
            grid[j][i] = float(data[j])

    # g2 = np.zeros(itersize)
    plt.plot(grid[int(itersize/2)])
    plt.title("Potential vs X Position; Y=0.5m")
    plt.xlabel("X(m/512)")
    plt.ylabel("Potential (V)")
    plt.show()