#include "geo.h"

namespace coordinates {

bool Coordinates::operator==(const Coordinates& other) const {
    return std::abs(lat - other.lat) < EPSILON && std::abs(lng - other.lng) < EPSILON;
}
bool Coordinates::operator!=(const Coordinates& other) const {
    return !(*this == other);
}

double ComputeDistance(Coordinates from, Coordinates to) {
    using namespace std;
    if (from == to) {
        return 0;
    }
    static const double dr = M_PI / 180.;
    return acos(sin(from.lat * dr) * sin(to.lat * dr)
                + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
        * EARTH_RADIUS;
}

} // namespace coordinates
