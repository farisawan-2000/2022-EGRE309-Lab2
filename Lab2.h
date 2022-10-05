#pragma once

// Configurable properties
using Decimal=float;
#define ITERCOUNT 100000
#define ITERSIZE 512


typedef enum SurfaceType {
    SURF_ROD = 0,
    SURF_CAPACITOR,
} SurfaceType;
typedef Decimal Vector2 __attribute__ ((vector_size (sizeof(Decimal) * 2)));

// 1m x 1m
#define GRID_TO_METERS(d) ((Decimal)(d) / (Decimal)ITERSIZE)

struct Surface {
    SurfaceType type;
    Vector2 position;
    Decimal charge;

    // if rod
    Decimal radius;

    // if capacitor or square
    Decimal width;
    Decimal height;
};




