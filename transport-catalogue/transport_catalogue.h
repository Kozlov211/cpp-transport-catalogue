#pragma once
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

#include "geo.h"

namespace transport_catalogue {

namespace stop {

struct Stop {
    std::string name;
    coordinates::Coordinates сoordinates;
};

struct CompStop {
    bool operator() (Stop* lhs, Stop* rhs) const;
};

} // namespace stop

namespace bus {

struct Bus {
    std::string name;
    std::vector<stop::Stop*> route;
    bool is_circle;
};

} // namespace bus

namespace RouteData {

struct RouteData {
    std::optional<size_t> stops;
    std::optional<size_t> uniq_stops;
    std::optional<double> geographic_legth;
    std::optional<double> road_length;
    std::optional<double> curvature_route;
};

} // namespace RouteData

namespace hash {

struct Hash {
    size_t operator() (const std::pair<stop::Stop*, stop::Stop*>& pair) const noexcept;
};

} // namespace Hash

using namespace stop;
using namespace bus;
using namespace hash;

class TransportCatalogue {
public:
    TransportCatalogue() = default;

    void AppendStop(std::string_view name, Stop* stop);

    void AppendBusToStop(Stop* stop, Bus* bus);

    void AppendDistanceToStop(Stop* name, Stop* stop, uint32_t distance);

    void AppendBus(std::string_view name, Bus* bus);

    Stop* GetStop(std::string_view name);

    Bus* GetBus(std::string_view name);

    size_t GetNumberStopsOnTheRoute(Bus* bus);

    size_t GetNumberUniqueStopsOnTheRoute(Bus* bus);

    bool CheckBus(std::string_view bus);

    bool CheckStop(std::string_view stop);

    double GetGeographicLength(Bus* bus);

    double GetRoadLength(Bus* bus);

    double GetCurvatureRoute(Bus* bus);

    std::set<std::string_view>& GetBusesPassingTheStop(std::string_view stop);

    const std::unordered_map<std::string_view, Bus*>& GetBuses() const;

    const std::unordered_map<std::string_view, Stop*>& GetStops() const;

private:
    std::unordered_map<std::string_view, Stop*> stops_;
    std::unordered_map<std::string_view, Bus*> buses_;
    std::unordered_map<Bus*, RouteData::RouteData> route_data;
    std::unordered_map<std::pair<Stop*, Stop*>, uint32_t, Hash> distance_between_stops_;
    std::unordered_map<std::string_view, std::set<std::string_view>> buses_passing_through_stop_;

private:
    uint32_t GetDistanceToStop(Stop* name, Stop* stop);

    void CountBusStopsOnTheRoute(Bus* bus);

    void CountBusUniqueStopsOnTheRoute(Bus* bus);

    void CountGeographicLength(Bus* bus);

    void CountRoadLength(Bus* bus);

    void CountCurvatureRoute(Bus* bus);
};

} // namespace transport_catalogue




