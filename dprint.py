import sys, os

with open("data", "r") as f:
    x = sys.argv[1]
    y = sys.argv[2]

    fl = f.readlines()[2:]
    for i, xx in enumerate(fl):
        for j, yy in enumerate(x.split()):
            if i == x and j == y:
                print(yy)
                exit()

