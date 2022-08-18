#pragma once
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>
#include <optional>

#include "geo.h"

namespace TransportCatalogue {

namespace Stop {

struct Stop {
    std::string name;
    Coordinates сoordinates;
};

struct CompStop {
    bool operator() (Stop* lhs, Stop* rhs) const;
};

} // namespace Stop

namespace Bus {

struct Bus {
    std::string name;
    std::vector<Stop::Stop*> route;
    bool is_circle;
};

} // namespace Bus

namespace RouteData {

struct RouteData {
    std::optional<size_t> stops;
    std::optional<size_t> uniq_stops;
    std::optional<double> geographic_legth;
    std::optional<double> road_length;
    std::optional<double> curvature_route;
};

} // namespace RouteData

namespace Hash {

struct Hash {
    size_t operator() (const std::pair<Stop::Stop*, Stop::Stop*>& pair) const noexcept;
};

} // namespace Hash

class TransportCatalogue {
public:
    TransportCatalogue() = default;

    void AppendStop(std::string_view name, Stop::Stop* stop); // Добавление остановки

    void AppendBusToStop(Stop::Stop* stop, Bus::Bus* bus); // Добавление автобуса к остановки

    void AppendDistanceToStop(Stop::Stop* name, Stop::Stop* stop, uint32_t distance); // Добавление расстояние между остановками

    void AppendBus(std::string_view name, Bus::Bus* bus); // Добавление маршрута

    Stop::Stop* GetStop(std::string_view name); // Получение остановки

    Bus::Bus* GetBus(std::string_view name); // Получение остановки

    size_t GetNumberStopsOnTheRoute(Bus::Bus* bus); // Количество остановок на маршруте

    size_t GetNumberUniqueStopsOnTheRoute(Bus::Bus* bus); // Количество уникальных остановок

    bool CheckBus(std::string_view bus); // Наличие остановки

    bool CheckStop(std::string_view stop); // Наличие автобуса

    double GetGeographicLength(Bus::Bus* bus); // Подсчет географического расстояния

    double GetRoadLength(Bus::Bus* bus); // Подсчет дорожного расстояния

    double GetCurvatureRoute(Bus::Bus* bus); // Получения кривизны маршрута

    std::set<std::string_view>& GetBusesPassingTheStop(std::string_view stop);

    const std::unordered_map<std::string_view, Bus::Bus*>& GetBuses() const;

    const std::unordered_map<std::string_view, Stop::Stop*>& GetStops() const;

private:
    std::unordered_map<std::string_view, Stop::Stop*> stops_; // Остановки
    std::unordered_map<std::string_view, Bus::Bus*> buses_; // Автобусы
    std::unordered_map<Bus::Bus*, RouteData::RouteData> route_data; // Информация о маршруте
    std::unordered_map<std::pair<Stop::Stop*, Stop::Stop*>, uint32_t, Hash::Hash> distance_between_stops_; // Дистанция между остановками
    std::unordered_map<std::string_view, std::set<std::string_view>> buses_passing_through_stop_; // Автобусы, проходящие через остановку

private:
    uint32_t GetDistanceToStop(Stop::Stop* name, Stop::Stop* stop);

    void CountBusStopsOnTheRoute(Bus::Bus* bus);

    void CountBusUniqueStopsOnTheRoute(Bus::Bus* bus);

    void CountGeographicLength(Bus::Bus* bus);

    void CountRoadLength(Bus::Bus* bus);

    void CountCurvatureRoute(Bus::Bus* bus);
};

} // namespace TransportCatalogue





