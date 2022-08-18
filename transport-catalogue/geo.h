#pragma once

#include <cmath>

inline const double EPSILON = 1e-6;

struct Coordinates {
    double lat;
    double lng;
    bool operator==(const Coordinates& other) const;
    bool operator!=(const Coordinates& other) const;
};

double ComputeDistance(Coordinates from, Coordinates to);
