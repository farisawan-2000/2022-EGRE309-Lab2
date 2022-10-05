default: data

CPPFLAGS := -march=native -mtune=native -Os -fopenmp -std=c++20 -lfmt

Lab2: Lab2.h

data: Lab2
	./Lab2

plot: data
	python3 plot.py data


clean:
	rm -f data Lab2
