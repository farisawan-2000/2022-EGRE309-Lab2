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

    grad = np.gradient(grid)

    # we want the negative gradient
    for i in range(np.shape(grad)[1]):
        for j in range(np.shape(grad)[2]):
            grad[0][i][j] *= -1
            grad[1][i][j] *= -1

    # second gradient
    grad2 = np.gradient(grid)
    # sample every (n/8)th point
    for i in range(np.shape(grad)[1]):
        for j in range(np.shape(grad)[2]):

            if i % 8 != 0 or j % 8 != 0:
                grad2[0][i][j] = 0
                grad2[1][i][j] = 0
            else:
                vm = sqrt((grad[0][i][j] ** 2) + (grad[1][i][j] ** 2))

                grad2[0][i][j] = grad[0][i][j] / vm
                grad2[1][i][j] = grad[1][i][j] / vm

    # create graph
    plt.contourf(grid, levels=100)
    plt.colorbar()
    plt.title("Potential + field for %s iterations, %dx%d grid" % (itercount, itersize, itersize))

    # quiver plot for visualizing data
    # plt.quiver(range(0, itersize), range(0, itersize), grad2[1], grad2[0], scale=64, minlength=0.001)# color='red', alpha=1)#, width=1/256.0)

    # stream plot for pretty graph
    # plt.streamplot(range(0, itersize), range(0, itersize), grad[1], grad[0], color="white")
    plt.show()
