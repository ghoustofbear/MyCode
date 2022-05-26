#include "utility.h"
#include <math.h>

double
util_round(double d)
{
    return ((int)(d * 1000 + 0.5)) / 1000.0;
}

double
util_distance(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

