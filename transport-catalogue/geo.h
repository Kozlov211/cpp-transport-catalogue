#pragma once

#include <cstdint>
#include <cmath>

namespace coordinates {

inline const double EPSILON = 1e-6;
inline const uint32_t EARTH_RADIUS = 6371000;

struct Coordinates {
    double lat = 0;
    double lng = 0;
    bool operator==(const Coordinates& other) const;
    bool operator!=(const Coordinates& other) const;
};

double ComputeDistance(Coordinates from, Coordinates to);

} // namespace coordinates
