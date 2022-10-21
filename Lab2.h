#pragma once

// Configurable properties
#define use_floats // comment to use higher precision doubles
#define ITERSIZE 512


#ifdef use_floats
    typedef float Decimal;
    #define pow powf
    #define sqrt sqrtf
    #define abs fabsf
#else
    typedef double Decimal;
#endif

#define e0 (8.854e-12)
#define m_k (1.0 / 4 * M_PI * e0)

typedef enum SurfaceType {
    SURF_ROD = 0,
    SURF_CAPACITOR,
} SurfaceType;
typedef Decimal Vector2 __attribute__ ((vector_size (sizeof(Decimal) * 2)));

// 1m x 1m
#define SCALE 1.0
#define GRID_TO_METERS(d) ((Decimal)(d) / (Decimal)ITERSIZE / (Decimal)SCALE)

typedef struct Surface {
    SurfaceType type;
    Vector2 position;
    Decimal charge;

    // if rod
    Decimal radius;

    // if capacitor or square
    Decimal width;
    Decimal height;
} Surface;

typedef struct _MSE {
    Decimal significand;
    int exponent;
} MSE;



