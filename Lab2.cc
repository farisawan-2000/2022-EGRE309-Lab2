#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <fmt/format.h>
#include <fmt/os.h>
#include <cfloat>

#include "Lab2.h"


Decimal grid[2][ITERSIZE][ITERSIZE]; // double buffered
unsigned bufIndex = 0;

#define CHARGE 1.602e-19
// #define CHARGE 1

Surface leftSurface = {
    .type = SURF_ROD,
    .position = {0.4, 0.5},
    .charge = -CHARGE,
    .radius = 0.05,

    // .width = 1e-3,
    // .height = 0.4,
};

Surface rightSurface = {
    .type = SURF_ROD,
    .position = {0.6, 0.5},
    .charge = CHARGE,
    .radius = 0.05,

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

Decimal vlen(Vector2 v) {
    return sqrtf(powf(v[0], 2) + powf(v[1], 2));
}

Decimal getInitialPotential(Surface *s, int x, int y) {
    Vector2 p = {x, y};

    return m_k * s->charge / (powf(vlen(p - s->radius), 2));
    // return s->charge;
}

Decimal mse() {
    Decimal ret = 0;

    for (int i = 0; i < ITERSIZE; i++) {
        for (int j = 0; j < ITERSIZE; j++) {
            Decimal vdiff = powf(grid[bufIndex][i][j] - grid[bufIndex ^ 1][i][j], 2);
            ret += vdiff;
        }
    }
    return ret;

    ret /= ITERSIZE * ITERSIZE;

    return sqrt(ret);
}


bool checkConvergence() {
    if (mse() < 0.0001) {
        return true;
    }
    return false;
}

Decimal find_max_diff(void) {
    Decimal max = 0;
    for (int i = 0; i < ITERSIZE; i++) {
        for (int j = 0; j < ITERSIZE; j++) {
            if (fabs(grid[bufIndex][i][j] - grid[bufIndex ^ 1][i][j]) > max) {
                max = fabs(grid[bufIndex][i][j] - grid[bufIndex ^ 1][i][j]);
            }
        }
    }

    return max;
}

Decimal getRand() {
    int x = rand() % 65535;

    return ((Decimal)x / 65535.0) * 1.602e-19;
}

int main(void) {
    auto out = fmt::output_file("data");

    Surface *s1 = &leftSurface;
    Surface *s2 = &rightSurface;

    // establish initial conditions: inside charge = ?, outside boundary = 0
    for (int j = 0; j < ITERSIZE; j++) {
        for (int k = 0; k < ITERSIZE; k++) {
            if (isInCharge(s1, j, k)) {
                grid[bufIndex][j][k] = getInitialPotential(s1, j, k);
            } else if (isInCharge(s2, j, k)) {
                grid[bufIndex][j][k] = getInitialPotential(s2, j, k);
            } else {
                grid[bufIndex][j][k] = getRand();
            }
        }
    }

    // iterate
    int i;
    for (i = 0; ; i++) {
        int maxdx = 0, maxdy = 0;
        // use all available cores for this operation
        #pragma omp parallel for
        for (int j = 0; j < ITERSIZE; j++) {
            for (int k = 0; k < ITERSIZE; k++) {
                if ((!isInCharge(s1, j, k)) && (!isInCharge(s2, j, k))) {
                    finite_method(j, k);
                }
            }
        }

        // #pragma omp single 
        if (i != 0 && i % 1000 == 0) {
            static Decimal old_mse = CHARGE;
            Decimal d = mse();
            fmt::print("{}: {}\n", i, d);
            if (abs(d - old_mse) < 0.0001) {
                break;
            } else {
                old_mse = d;
            }
        }

        bufIndex ^= 1;
    }

    out.print("{}\n", i);
    out.print("{}\n", ITERSIZE);

    fmt::print("Last diff is {}\n", find_max_diff());

    // print to file
    for (int j = 0; j < ITERSIZE; j++) {
        for (int k = 0; k < ITERSIZE; k++) {
            out.print("{} ", grid[bufIndex][j][k]);
        }
        out.print("\n");
    }

    return 0;
}


