#include "transport_catalogue.h"

using namespace std;

namespace TransportCatalogue {

void TransportCatalogue::AddStop(const string& name, Coordinates сoordinates, const bool is_empty) {
    if (is_empty) {
        stops_[name];
    } else {
        stops_[name].сoordinates = сoordinates;
    }
}

void TransportCatalogue::AddBusToStop (const string& name, string_view bus) {
    stops_[name].buses.insert(bus);
}

void TransportCatalogue::AddDistanceToStop (const string& name, string_view stop, double distance) {
    stops_[name].distance_to_stop[stop] = distance;
}

void TransportCatalogue::AddBusRoute(const string& name, const bool is_circle, vector<string_view> route) {
    buses_[name] = {is_circle, route};
}

Stop::Stop TransportCatalogue::GetStop(const string& name) {
    return stops_[name];
}

std::vector<string_view> TransportCatalogue::GetBusRoute(const string& name) {
    return buses_[name].route;
}

double TransportCatalogue::CountGeographicDistance(const string& name) {
    Bus::Bus& bus = buses_[name];
    double route_length = 0;
    for (size_t i = 0; i < bus.route.size() - 1; ++i) {
        route_length += ComputeDistance(GetStop(string(bus.route[i])).сoordinates, GetStop(string(bus.route[i + 1])).сoordinates);
    }
    return bus.is_circle ? route_length : route_length * 2;
}

size_t TransportCatalogue::BusStopCount(const string& name) {
    Bus::Bus& bus = buses_[name];
    return bus.is_circle ? bus.route.size() : bus.route.size() * 2 - 1;

}

size_t TransportCatalogue::BusUniqStopCount(const string& name) {
    vector<string_view> route = GetBusRoute(name);
    unordered_set<string_view> result = {route.begin(), route.end()};
    return result.size();
}

unordered_map<string, Stop::Stop>::iterator TransportCatalogue::GetKeyStop(const string& name) {
    return stops_.find(name);
}

unordered_map<std::string, Bus::Bus>::iterator TransportCatalogue::GetKeyBus(const string& name) {
    return buses_.find(name);
}

bool TransportCatalogue::CheckBus(const string& name) const {
    return buses_.count(name);
}

bool TransportCatalogue::CheckStop(const string& name) const {
    return stops_.count(name);
}

double TransportCatalogue::GetDistanceToStop(const string& name, string_view stop) {
    Stop::Stop& stop_ = stops_[name];
    if (stop_.distance_to_stop.count(stop)) {
        return stop_.distance_to_stop[stop];
    } else {
        return stops_[string(stop)].distance_to_stop[name];
    }
}

double TransportCatalogue::CountRoadDistance(const string& name) {
    double route_length = 0;
    Bus::Bus& bus = buses_[name];
    if (bus.is_circle) {
        for (size_t i = 0; i < bus.route.size() - 1; ++i) {
            route_length += GetDistanceToStop(string(bus.route[i]), string(bus.route[i + 1]));
        }
        return route_length;
    } else {
        for (size_t i = 0; i < bus.route.size() - 1; ++i) {
            route_length += GetDistanceToStop(string(bus.route[i]), string(bus.route[i + 1]));
            route_length += GetDistanceToStop(string(bus.route[bus.route.size() - 1 - i]), string(bus.route[bus.route.size() - 1 - i - 1]));
        }
        return route_length;
    }
}

double TransportCatalogue::GetCurvatureRoute(const string& name) {
    return CountRoadDistance(name) / CountGeographicDistance(name);
}

} // namespace TransportCatalogue

