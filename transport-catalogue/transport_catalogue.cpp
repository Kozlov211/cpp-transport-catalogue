#include "transport_catalogue.h"

#include <transport_catalogue.pb.h>

namespace transport_catalogue {
using namespace std;
using namespace domain;

void TransportCatalogue::AppendStop(domain::Stop&& stop) {
    deque_stops_.push_back(move(stop));
    Stop& current_stop = deque_stops_.back();
    stops_[current_stop.name] = &current_stop;
}


void TransportCatalogue::AppendBus(Bus&& bus) {
    deque_buses_.push_back(move(bus));
    Bus& current_bus = deque_buses_.back();
    buses_[current_bus.name] = &current_bus;
    for (auto stop : current_bus.route) {
        AppendBusToStop(stop, &current_bus);
    }
}

void TransportCatalogue::AppendBusToStop (Stop* stop, Bus* bus) {
    buses_passing_through_stop_[stop->name].insert(bus->name);
}

void TransportCatalogue::AppendDistanceToStop (Stop* from_stop, Stop* to_stop, size_t distance) {
    distance_between_stops_[{from_stop, to_stop}] = distance;
}

Stop* TransportCatalogue::GetStop(string_view name) const {
    return stops_.at(name);
}

Bus* TransportCatalogue::GetBus(string_view name) const {
    return buses_.at(name);
}

size_t TransportCatalogue::GetNumberStopsOnTheRoute(Bus* bus) const {
    if (route_data_[bus].stops) {
        return *route_data_[bus].stops;
    }
    CountBusStopsOnTheRoute(bus);
    return *route_data_[bus].stops;
}

void TransportCatalogue::CountBusStopsOnTheRoute(Bus* bus) const {
    route_data_[bus].stops = bus->is_circle ? bus->route.size() : bus->route.size() * 2 - 1;
}

size_t TransportCatalogue::GetNumberUniqueStopsOnTheRoute(Bus* bus) const {
    if (route_data_[bus].uniq_stops) {
        return *route_data_[bus].uniq_stops;
    }
    CountBusUniqueStopsOnTheRoute(bus);
    return *route_data_[bus].uniq_stops;
}

void TransportCatalogue::CountBusUniqueStopsOnTheRoute(Bus* bus) const {
    unordered_set<Stop*> unique_stops = {bus->route.begin(), bus->route.end()};
    route_data_[bus].uniq_stops = unique_stops.size();
}

bool TransportCatalogue::CheckBus(string_view bus) const {
    return buses_.count(bus);
}

bool TransportCatalogue::CheckStop(string_view  stop) const {
    return stops_.count(stop);
}

double TransportCatalogue::GetGeographicLength(Bus* bus) const {
    if (route_data_[bus].geographic_legth) {
        return *route_data_[bus].geographic_legth;
    }
    CountGeographicLength(bus);
    return *route_data_[bus].geographic_legth;
}

void TransportCatalogue::CountGeographicLength(Bus* bus) const {
    double route_length = 0;
    for (size_t i = 0; i < bus->route.size() - 1; ++i) {
        route_length += ComputeDistance(GetStop(bus->route[i]->name)->сoordinates, GetStop(bus->route[i  + 1]->name)->сoordinates);
    }
    route_data_[bus].geographic_legth = bus->is_circle ? route_length : route_length * 2;
}

uint32_t TransportCatalogue::GetDistanceToStop(Stop* from_stop, Stop* to_stop) const {
    if (distance_between_stops_.count({from_stop, to_stop})) {
        return distance_between_stops_.at({from_stop, to_stop});
    } else {
        return distance_between_stops_.at({to_stop, from_stop});
    }
}

double TransportCatalogue::GetRoadLength(Bus* bus) const {
        if (route_data_.count(bus) && route_data_[bus].road_length) {
            return *route_data_[bus].road_length;
        }
        CountRoadLength(bus);
        return *route_data_[bus].road_length;
}

void TransportCatalogue::CountRoadLength(Bus* bus) const {
    double route_length = 0;
    if (bus->is_circle) {
        for (size_t i = 0; i < bus->route.size() - 1; ++i) {
            route_length += GetDistanceToStop(bus->route[i], bus->route[i + 1]);
        }
        route_data_[bus].road_length = route_length;
    } else {
        for (size_t i = 0; i < bus->route.size() - 1; ++i) {
            route_length += GetDistanceToStop(bus->route[i], bus->route[i + 1]);
            route_length += GetDistanceToStop(bus->route[bus->route.size() - 1 - i], bus->route[bus->route.size() - 1 - i - 1]);
        }
        route_data_[bus].road_length = route_length;
    }
}

double TransportCatalogue::GetCurvatureRoute(Bus* bus) const {
    if (route_data_[bus].curvature_route) {
        return *route_data_[bus].curvature_route;
    }
    CountCurvatureRoute(bus);
    return *route_data_[bus].curvature_route;
}

void TransportCatalogue::CountCurvatureRoute(Bus* bus) const {
    route_data_[bus].curvature_route = GetRoadLength(bus) / GetGeographicLength(bus);
}

const set<string_view>& TransportCatalogue::GetBusesPassingTheStop(string_view stop) const {
    static set<string_view> empty = {};
    return buses_passing_through_stop_.count(stop) ? buses_passing_through_stop_.at(stop) : empty;
}

const std::unordered_map<std::string_view, Bus*>& TransportCatalogue::GetBuses() const {
    return buses_;
}

const std::unordered_map<std::string_view, Stop*>& TransportCatalogue::GetStops() const {
    return stops_;
}

const std::unordered_map<std::pair<Stop*, Stop*>, size_t, detail::Hash>& TransportCatalogue::GetDistanceBetweenStops() const {
    return distance_between_stops_;
}


const std::deque<Bus>& TransportCatalogue::GetDequeBuses() const {
    return deque_buses_;
}

const std::deque<Stop>& TransportCatalogue::GetDequeStops() const {
    return deque_stops_;
}

namespace detail {

size_t Hash::operator() (const std::pair<Stop*, Stop*>& pair) const noexcept {
    std::size_t h1 = std::hash<std::uint32_t>{}(reinterpret_cast<size_t>(pair.first));
    std::size_t h2 = std::hash<std::uint32_t>{}(reinterpret_cast<size_t>(pair.second));
    return h1 ^ (h2 << 1);
}

bool CompStop::operator() (Stop* lhs, Stop* rhs)const{
    return lhs->name < rhs->name;
}

} // namespace detail

} // namespace transport_catalogue
