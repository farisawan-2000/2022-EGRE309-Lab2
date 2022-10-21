default: data

PROGRAM := Lab2
OPT := -Os
CPPFLAGS := -march=native -fno-stack-protector -ffast-math -mtune=native $(OPT) -fopenmp -std=c++20 -lfmt

$(PROGRAM): Lab2.h

data: $(PROGRAM)
	./$(PROGRAM)

plot: data
	python3 plot.py data


clean:
	rm -f data $(PROGRAM)
