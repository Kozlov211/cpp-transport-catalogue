#include "transport_catalogue.h"

using namespace std;

namespace transport_catalogue {

using namespace stop;
using namespace bus;
using namespace hash;

void TransportCatalogue::AppendStop(string_view name, Stop* stop) {
    stops_[name] = stop;
}

void TransportCatalogue::AppendBusToStop (Stop* stop, Bus* bus) {
    buses_passing_through_stop_[stop->name].insert(bus->name);
}

void TransportCatalogue::AppendDistanceToStop (Stop* name, Stop* stop, uint32_t distance) {
    distance_between_stops_[{name, stop}] = distance;
}

void TransportCatalogue::AppendBus(string_view name, Bus* bus) {
    buses_[name] = bus;
}

Stop* TransportCatalogue::GetStop(string_view name) {
    static Stop* empty;
    return CheckStop(name) ? stops_.at(name) : empty;
}

Bus* TransportCatalogue::GetBus(string_view name) {
    static Bus* empty;
    return CheckBus(name) ? buses_.at(name) : empty;
}

size_t TransportCatalogue::GetNumberStopsOnTheRoute(Bus* bus) {
    if (route_data[bus].stops) {
        return *route_data[bus].stops;
    }
    CountBusStopsOnTheRoute(bus);
    return *route_data[bus].stops;
}

void TransportCatalogue::CountBusStopsOnTheRoute(Bus* bus) {
    route_data[bus].stops = bus->is_circle ? bus->route.size() : bus->route.size() * 2 - 1;
}

size_t TransportCatalogue::GetNumberUniqueStopsOnTheRoute(Bus* bus) {
    if (route_data[bus].uniq_stops) {
        return *route_data[bus].uniq_stops;
    }
    CountBusUniqueStopsOnTheRoute(bus);
    return *route_data[bus].uniq_stops;
}

void TransportCatalogue::CountBusUniqueStopsOnTheRoute(Bus* bus) {
    unordered_set<Stop*> unique_stops = {bus->route.begin(), bus->route.end()};
    route_data[bus].uniq_stops = unique_stops.size();
}

bool TransportCatalogue::CheckBus(string_view bus) {
    return buses_.count(bus);
}

bool TransportCatalogue::CheckStop(string_view  stop) {
    return stops_.count(stop);
}

double TransportCatalogue::GetGeographicLength(Bus* bus) {
    if (route_data[bus].geographic_legth) {
        return *route_data[bus].geographic_legth;
    }
    CountGeographicLength(bus);
    return *route_data[bus].geographic_legth;
}

void TransportCatalogue::CountGeographicLength(Bus* bus) {
    double route_length = 0;
    for (size_t i = 0; i < bus->route.size() - 1; ++i) {
        route_length += ComputeDistance(GetStop(bus->route[i]->name)->сoordinates, GetStop(bus->route[i  + 1]->name)->сoordinates);
    }
    route_data[bus].geographic_legth = bus->is_circle ? route_length : route_length * 2;
}

uint32_t TransportCatalogue::GetDistanceToStop(Stop* name, Stop* stop) {
    if (distance_between_stops_.count({name, stop})) {
        return distance_between_stops_[{name, stop}];
    } else {
        return distance_between_stops_[{stop, name}];
    }
}

double TransportCatalogue::GetRoadLength(Bus* bus) {
        if (route_data.count(bus) && route_data[bus].road_length) {
            return *route_data[bus].road_length;
        }
        CountRoadLength(bus);
        return *route_data[bus].road_length;
}

void TransportCatalogue::CountRoadLength(Bus* bus) {
    double route_length = 0;
    if (bus->is_circle) {
        for (size_t i = 0; i < bus->route.size() - 1; ++i) {
            route_length += GetDistanceToStop(bus->route[i], bus->route[i + 1]);
        }
        route_data[bus].road_length = route_length;
    } else {
        for (size_t i = 0; i < bus->route.size() - 1; ++i) {
            route_length += GetDistanceToStop(bus->route[i], bus->route[i + 1]);
            route_length += GetDistanceToStop(bus->route[bus->route.size() - 1 - i], bus->route[bus->route.size() - 1 - i - 1]);
        }
        route_data[bus].road_length = route_length;
    }
}

double TransportCatalogue::GetCurvatureRoute(Bus* bus) {
    if (route_data[bus].curvature_route) {
        return *route_data[bus].curvature_route;
    }
    CountCurvatureRoute(bus);
    return *route_data[bus].curvature_route;
}

void TransportCatalogue::CountCurvatureRoute(Bus* bus) {
    route_data[bus].curvature_route = GetRoadLength(bus) / GetGeographicLength(bus);
}

set<string_view>& TransportCatalogue::GetBusesPassingTheStop(string_view stop) {
    static set<string_view> empty;
    return buses_passing_through_stop_.count(stop) ? buses_passing_through_stop_.at(stop) : empty;
}

const std::unordered_map<std::string_view, Bus*>& TransportCatalogue::GetBuses() const {
    return buses_;
}

const std::unordered_map<std::string_view, Stop*>& TransportCatalogue::GetStops() const {
    return stops_;
}

size_t Hash::operator() (const std::pair<stop::Stop*, stop::Stop*>& pair) const noexcept {
    std::size_t h1 = std::hash<std::uint32_t>{}(reinterpret_cast<size_t>(pair.first));
    std::size_t h2 = std::hash<std::uint32_t>{}(reinterpret_cast<size_t>(pair.second));
    return h1 ^ (h2 << 1);
}

namespace stop {

bool CompStop::operator() (Stop* lhs, Stop* rhs)const{
    return lhs->name < rhs->name;
}

} // namespace stop


} // namespace transport_catalogue
