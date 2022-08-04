#include "transport_catalogue.h"

using namespace std;

namespace TransportCatalogue {

void TransportCatalogue::AddStop(const string& name, Coordinates сoordinates) {
    stops_[name] = {сoordinates};
}

void TransportCatalogue::AddBusToStop (string_view name, string_view bus) {
    buses_passing_through_the_stop_[name].insert(bus);
}

void TransportCatalogue::SetDistanceToStop (const std::string& name, const std::string& stop, uint32_t distance) {
    auto name_key = GetKeyStop(name);
    auto stop_key = GetKeyStop(stop);
    distance_between_stops_[{name_key->first, stop_key->first}] = distance;
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

double TransportCatalogue::GetGeographicDistance(const string& name) {
    if (route_data.count(name) && route_data[name].geographic_distance.first) {
        return route_data[name].geographic_distance.second;
    }
    auto name_key = GetKeyBus(name);
    Bus::Bus& bus = buses_[name];
    double route_length = 0;
    for (size_t i = 0; i < bus.route.size() - 1; ++i) {
        route_length += ComputeDistance(GetStop(string(bus.route[i])).сoordinates, GetStop(string(bus.route[i + 1])).сoordinates);
    }
    route_data[name_key->first].geographic_distance.second = bus.is_circle ? route_length : route_length * 2;
    route_data[name_key->first].geographic_distance.first = true;
    return  route_data[name_key->first].geographic_distance.second;
}

size_t TransportCatalogue::GetBusStops(const string& name) {
    if (route_data.count(name) && route_data[name].stops.first) {
        return route_data[name].stops.second;
    }
    auto name_key = GetKeyBus(name);
    Bus::Bus& bus = buses_[name];
    route_data[name_key->first].stops.second = bus.is_circle ? bus.route.size() : bus.route.size() * 2 - 1;
    route_data[name_key->first].stops.first = true;
    return route_data[name_key->first].stops.second;

}

size_t TransportCatalogue::GetBusUniqStops(const string& name) {
    if (route_data.count(name) && route_data[name].uniq_stops.first) {
        return route_data[name].uniq_stops.second;
    }
    auto name_key = GetKeyBus(name);
    vector<string_view> route = GetBusRoute(name);
    unordered_set<string_view> result = {route.begin(), route.end()};
    route_data[name_key->first].uniq_stops.second = result.size();
    route_data[name_key->first].uniq_stops.first = true;
    return route_data[name_key->first].uniq_stops.second;
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


double TransportCatalogue::GetDistanceToStop(string_view name, string_view stop) {
    if (distance_between_stops_.count({name, stop})) {
        return distance_between_stops_[{name, stop}];
    } else {
        return distance_between_stops_[{stop, name}];
    }
}

double TransportCatalogue::GetRoadDistance(const string& name) {
    if (route_data.count(name) && route_data[name].road_distance.first) {
        return route_data[name].road_distance.second;
    }
    double route_length = 0;
    Bus::Bus& bus = buses_[name];
    auto name_key = GetKeyBus(name);
    if (bus.is_circle) {
        for (size_t i = 0; i < bus.route.size() - 1; ++i) {
            route_length += GetDistanceToStop(string(bus.route[i]), string(bus.route[i + 1]));
        }
        route_data[name_key->first].road_distance.second = route_length;
        route_data[name_key->first].road_distance.first = true;
        return route_length;
    } else {
        for (size_t i = 0; i < bus.route.size() - 1; ++i) {
            route_length += GetDistanceToStop(string(bus.route[i]), string(bus.route[i + 1]));
            route_length += GetDistanceToStop(string(bus.route[bus.route.size() - 1 - i]), string(bus.route[bus.route.size() - 1 - i - 1]));
        }
        route_data[name_key->first].road_distance.second = route_length;
        route_data[name_key->first].road_distance.first = true;
        return route_length;
    }
}

double TransportCatalogue::GetCurvatureRoute(const string& name) {
    if (route_data[name].curvature_route.first) {
        return route_data[name].curvature_route.second;
    }
    auto name_key = GetKeyBus(name);
    route_data[name_key->first].curvature_route.second = GetRoadDistance(name) / GetGeographicDistance(name);
    route_data[name_key->first].curvature_route.first = true;
    return route_data[name_key->first].curvature_route.second;
}

std::set<std::string_view> TransportCatalogue::GetBusesPassingTheStop(const std::string& name) {
    return buses_passing_through_the_stop_[name];
}

} // namespace TransportCatalogue

