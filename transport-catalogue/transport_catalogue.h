#pragma once

#include "geo.h"
#include "domain.h"

#include <deque>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

namespace transport_catalogue {

namespace detail {

struct CompStop {
    bool operator() (domain::Stop* lhs, domain::Stop* rhs) const;
};

struct Hash {
    size_t operator() (const std::pair<domain::Stop*, domain::Stop*>& pair) const noexcept;
};

} // namespace detail


class TransportCatalogue {
public:
    void AppendStop(domain::Stop&& stop);

    void AppendBus(domain::Bus&& bus);

    void AppendDistanceToStop(domain::Stop* from_stop, domain::Stop* to_stop, size_t distance);

    domain::Stop* GetStop(std::string_view name) const;

    domain::Bus* GetBus(std::string_view name) const;

    size_t GetNumberStopsOnTheRoute(domain::Bus* bus) const;

    size_t GetNumberUniqueStopsOnTheRoute(domain::Bus* bus) const;

    bool CheckBus(std::string_view bus) const;

    bool CheckStop(std::string_view stop) const;

    double GetGeographicLength(domain::Bus* bus) const;

    double GetRoadLength(domain::Bus* bus) const;

    double GetCurvatureRoute(domain::Bus* bus) const;

    const std::set<std::string_view>& GetBusesPassingTheStop(std::string_view stop) const;

    const std::unordered_map<std::string_view, domain::Bus*>& GetBuses() const;

    const std::unordered_map<std::string_view, domain::Stop*>& GetStops() const;

    const std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, size_t, detail::Hash>& GetDistanceBetweenStops() const;

    uint32_t GetDistanceToStop(domain::Stop* from_stop, domain::Stop* to_stop) const;

    const std::deque<domain::Bus>& GetDequeBuses() const;

    const std::deque<domain::Stop>& GetDequeStops() const;

private:
    std::unordered_map<std::string_view, domain::Stop*> stops_;
    std::unordered_map<std::string_view, domain::Bus*> buses_;
    mutable std::unordered_map<domain::Bus*, domain::RouteData> route_data_;
    std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, size_t, detail::Hash> distance_between_stops_;
    std::unordered_map<std::string_view, std::set<std::string_view>> buses_passing_through_stop_;
    std::deque<domain::Bus> deque_buses_;
    std::deque<domain::Stop> deque_stops_;
private:
    void AppendBusToStop(domain::Stop* stop, domain::Bus* bus);

    void CountBusStopsOnTheRoute(domain::Bus* bus) const;

    void CountBusUniqueStopsOnTheRoute(domain::Bus* bus) const;

    void CountGeographicLength(domain::Bus* bus) const;

    void CountRoadLength(domain::Bus* bus) const;

    void CountCurvatureRoute(domain::Bus* bus) const;
};

} // namespace transport_catalogue



