#pragma once

#include <cmath>

namespace coordinates {

inline const double EPSILON = 1e-6;

struct Coordinates {
    double lat = 0;
    double lng = 0;
    bool operator==(const Coordinates& other) const;
    bool operator!=(const Coordinates& other) const;
};

double ComputeDistance(Coordinates from, Coordinates to);

} // namespace coordinates
