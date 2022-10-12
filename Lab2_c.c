#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <omp.h>

#include "Lab2.h"


Decimal grid[2][ITERSIZE][ITERSIZE]; // double buffered
unsigned bufIndex = 0;

#define CHARGE 1.602e-19

Surface leftSurface = {
    .type = SURF_ROD,
    .position = {0.2, 0.5},
    .charge = -CHARGE,
    .radius = 0.1,

    // .width = 1e-3,
    // .height = 0.4,
};

Surface rightSurface = {
    .type = SURF_ROD,
    .position = {0.8, 0.5},
    .charge = CHARGE,
    .radius = 0.1,

    // .width = 1e-3,
    // .height = 0.4,
};

inline Decimal Vec2_Mag(Vector2 v) {
    return (v[0] * v[0]) + (v[1] * v[1]);
}
#define SQUARE(x) ((x) * (x))

bool isInCharge(Surface *s, int x, int y) {
    Decimal xx = GRID_TO_METERS(x);
    Decimal yy = GRID_TO_METERS(y);

    // if (s->type == SURF_CAPACITOR) {
    //     return ((abs(xx - s->position[0]) < s->width) && (abs(yy - s->position[1]) < s->height));
    // }

    return SQUARE(xx - s->position[0]) + SQUARE(yy - s->position[1]) < SQUARE(s->radius);

}


void finite_method(int x, int y) {
    Decimal result = 0;
    if (x > 0) {
        result += grid[bufIndex][x - 1][y];
    }
    if (x + 1 < ITERSIZE) {
        result += grid[bufIndex][x + 1][y];
    }

    if (y > 0) {
        result += grid[bufIndex][x][y - 1];
    }
    if (y + 1 < ITERSIZE) {
        result += grid[bufIndex][x][y + 1];
    }

    grid[bufIndex ^ 1][x][y] = result / 4.0;
}



int main(void) {
    FILE * out = fopen("data", "w+");

    fprintf(out, "%d\n", ITERCOUNT);
    fprintf(out, "%d\n", ITERSIZE);

    Surface *s1 = &leftSurface;
    Surface *s2 = &rightSurface;

    // establish initial conditions: inside charge = ?, outside boundary = 0
    for (int j = 0; j < ITERSIZE; j++) {
        for (int k = 0; k < ITERSIZE; k++) {
            if (isInCharge(s1, j, k)) {
                grid[bufIndex][j][k] = s1->charge;
            } else if (isInCharge(s2, j, k)) {
                grid[bufIndex][j][k] = s2->charge;
            } else {
                grid[bufIndex][j][k] = 0;
            }
        }
    }

    // iterate
    for (int i = 0; i < ITERCOUNT; i++) {
        // use all available cores for this operation
        #pragma omp parallel for
        for (int j = 0; j < ITERSIZE; j++) {
            for (int k = 0; k < ITERSIZE; k++) {
                if ((!isInCharge(s1, j, k)) && (!isInCharge(s2, j, k))) {
                    finite_method(j, k);
                }
            }
        }
        bufIndex ^= 1;
    }


    // print to file
    for (int j = 0; j < ITERSIZE; j++) {
        for (int k = 0; k < ITERSIZE; k++) {
            fprintf(out, "%e ", grid[bufIndex][j][k]);
        }
        fprintf(out, "\n");
    }

    return 0;
}


