#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <fmt/format.h>
#include <fmt/os.h>
#include <signal.h>
#include <cfloat>

#include "Lab2.h"

// double buffered
Decimal grid[2][ITERSIZE][ITERSIZE];
unsigned bufIndex = 0;
unsigned numIterations = 0;

// Set Charge and Radius here
#define CHARGE 1
#define RADIUS 0.05

// Determines when the simulation stops
MSE Thresholds[] = {
    [SURF_ROD] = {
        .significand = 5.0,
        .exponent = -15,
    },
    [SURF_CAPACITOR] = {
        .significand = 2.0,
        .exponent = -11,
    },
}

/************ ROD SURFACES *****************/
// Surface leftSurface = {
//     .type = SURF_ROD,
//     .position = {0.4, 0.5},
//     .charge = -CHARGE,
//     .radius = RADIUS,
// };

// Surface rightSurface = {
//     .type = SURF_ROD,
//     .position = {0.6, 0.5},
//     .charge = CHARGE,
//     .radius = RADIUS,
// };

/************ CAPACITOR SURFACES ***********/
Surface leftSurface = {
    .type = SURF_CAPACITOR,
    .position = {0.499, 0.5},
    .charge = -CHARGE,
    .width = 1e-3,
    .height = 0.1,
};

Surface rightSurface = {
    .type = SURF_CAPACITOR,
    .position = {0.501, 0.5},
    .charge = CHARGE,
    .width = 1e-3,
    .height = 0.1,
};

inline Decimal Vec2_Mag(Vector2 v) {
    return (v[0] * v[0]) + (v[1] * v[1]);
}
#define SQUARE(x) ((x) * (x))

bool isInCharge(Surface *s, int x, int y) {
    Decimal xx = GRID_TO_METERS(x);
    Decimal yy = GRID_TO_METERS(y);

    if (s->type == SURF_CAPACITOR) {
        return ((fabs(xx - s->position[0]) < s->width) && (fabs(yy - s->position[1]) < s->height));
    }

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
    return sqrt(pow(v[0], 2) + pow(v[1], 2));
}

Decimal getInitialPotential(Surface *s, int x, int y) {
    if (s->type == SURF_CAPACITOR) {
        return m_k * s->charge / (pow(s->width/2, 2));
    }

    return m_k * s->charge / (pow(s->radius, 2));
}

Decimal mse(int *exp) {
    Decimal ret = 0;

    for (int i = 0; i < ITERSIZE; i++) {
        for (int j = 0; j < ITERSIZE; j++) {
            Decimal vdiff = pow(grid[bufIndex][i][j] - grid[bufIndex ^ 1][i][j], 2);
            ret += vdiff;
        }
    }

    ret /= ITERSIZE * ITERSIZE;

    // Hacky way to split significand and exponent
    char buf[50]; // allocate buffer
    sprintf(buf, "%e", sqrt(ret)); // prints e.g. 1.0e-2 for 0.01
    char *p = buf;
    // split the string at the occurrence of 'e'
    while (*p) {
        if (*p == 'e') {
            *p = 0;
            // set exponent
            *exp = atoi(p + 1);
        }
        p++;
    }

    // return significand
    return atof(buf);
}

Decimal getRand() {
    int x = rand() % 65535;

    // Decimal max = (m_k * CHARGE / (pow(RADIUS, 2)));
    Decimal max = 0;

    return ((Decimal)x / 65535.0) * max;
}

// Establish Output file (faster with fmtlib)
auto out = fmt::output_file("data");

void writeData() {
    out.print("{}\n", numIterations);
    out.print("{}\n", ITERSIZE);

    // print to file
    for (int j = 0; j < ITERSIZE; j++) {
        for (int k = 0; k < ITERSIZE; k++) {
            out.print("{} ", grid[bufIndex][j][k]);
        }
        out.print("\n");
    }
    exit(0);
}

int main(void) {

    Surface *s1 = &leftSurface;
    Surface *s2 = &rightSurface;

    // establish initial conditions: inside charge = V, outside boundary = 0
    for (int j = 0; j < ITERSIZE; j++) {
        for (int k = 0; k < ITERSIZE; k++) {
            if (isInCharge(s1, j, k)) {
                grid[bufIndex][j][k] =
                grid[bufIndex ^ 1][j][k] =
                    getInitialPotential(s1, j, k);
            } else if (isInCharge(s2, j, k)) {
                grid[bufIndex][j][k] =
                grid[bufIndex ^ 1][j][k] =
                    getInitialPotential(s2, j, k);
            } else {
                grid[bufIndex][j][k] =
                grid[bufIndex ^ 1][j][k] =
                    getRand();
            }
        }
    }

    // Debug: Press Ctrl-C to exit and write data as-is
    signal(SIGINT, (__sighandler_t)writeData);

    // iterate
    for (numIterations = 0; ; numIterations++) {
        // use all available cores for this operation
        #pragma omp parallel for
        for (int j = 0; j < ITERSIZE; j++) {
            for (int k = 0; k < ITERSIZE; k++) {
                if ((!isInCharge(s1, j, k)) && (!isInCharge(s2, j, k))) {
                    finite_method(j, k);
                }
            }
        }

        // Swap Buffers
        bufIndex ^= 1;

        // Check Mean Squared Error
        if (numIterations != 0 && numIterations % 1000 == 0) {
            int exp;
            
            Decimal d = mse(&exp);
            fmt::print("{}: {}e{}\n", numIterations, d, exp);
            if (d <= Thresholds[s1->type].significand && exp <= Thresholds[s1->type].exponent) {
                break;
            }
        }
    }

    writeData();

    return 0;
}


