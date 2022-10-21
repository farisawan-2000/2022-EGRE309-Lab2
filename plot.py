import sys, os
import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from math import sqrt

# https://www.mathworks.com/help/symbolic/divergence.html
# https://stackoverflow.com/a/43759790
def divergence(f):
    """
    Computes the divergence of the vector field f, corresponding to dFx/dx + dFy/dy + ...
    :param f: List of ndarrays, where every item of the list is one dimension of the vector field
    :return: Single ndarray of the same shape as each of the items in f, which corresponds to a scalar field
    """
    num_dims = len(f)
    return np.ufunc.reduce(np.add, [np.gradient(f[i], axis=i) for i in range(num_dims)])

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

    # The Laplacian is the divergence of the gradient
    grad2_full = np.gradient(grid)
    retval = divergence(grad2_full)

    # create graph
    plt.contourf(grid, levels=100)
    plt.colorbar()
    plt.title("Potential + field for %s iterations, %dx%d grid" % (itercount, itersize, itersize))

    # stream plot for pretty graph
    plt.streamplot(range(0, itersize), range(0, itersize), grad[1], grad[0], color="white")

    plt.figure(2)
    plt.contourf(retval, levels=100)
    plt.colorbar()
    plt.title("Poisson's Equation, ∇²V = ∇⋅∇(V) = 0")
    positive_charge = 0
    negative_charge = 0
    for i in range(np.shape(retval)[0]):
        for j in range(np.shape(retval)[1]):
            if retval[i][j] > 0:
                positive_charge += retval[i][j]
            if retval[i][j] < 0:
                negative_charge += retval[i][j]

    # Print error value results
    print("Sum of positive values:",positive_charge)
    print("Sum of negative values:",negative_charge)
    print("Total sum of values:", positive_charge + negative_charge)
    print("Error between positive and negative sums: "
        + str(100*(abs(positive_charge+negative_charge))/positive_charge)
        + "%"
    )
    plt.show()
