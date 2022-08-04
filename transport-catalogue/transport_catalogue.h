#pragma once
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"

namespace TransportCatalogue {

namespace Hash {

class Hash {
public:
    size_t operator() (const std::pair<std::string_view, std::string_view>& pair) const {
        size_t h_first = hasher_(pair.first);
        size_t h_second = hasher_(pair.second);
        return h_first * 37 + h_second * 37 * 37;
    }
private:
    std::hash<std::string_view> hasher_;
};

} // namespace Hash

namespace Stop {

struct Stop {
    Coordinates сoordinates;
};

} // namespace Stop

namespace Bus {

struct Bus {
    bool is_circle;
    std::vector<std::string_view> route;
};

} // namespace Bus

namespace RouteData {

struct RouteData {
    std::pair<bool, size_t> stops;
    std::pair<bool, size_t> uniq_stops;
    std::pair<bool, double> geographic_distance;
    std::pair<bool, double> road_distance;
    std::pair<bool, double> curvature_route;
};

}


class TransportCatalogue {
public:
    TransportCatalogue() = default;

    void AddStop(const std::string& name, Coordinates сoordinates); // Добавление остановки

    void AddBusToStop (std::string_view name, std::string_view bus); // Добавление автобуса к остановки

    void SetDistanceToStop (const std::string& name, const std::string& stop, uint32_t distance);

    void AddBusRoute(const std::string& name, const bool is_circle, std::vector<std::string_view> route); // Добавление маршрута

    Stop::Stop GetStop(const std::string& name); // Получение остановки

    std::vector<std::string_view> GetBusRoute(const std::string& name); // Получение маршрута автобуса

    double GetGeographicDistance(const std::string& name); // Подсчет географического расстояния

    size_t GetBusStops(const std::string& name); // Количество остановок на маршруте

    size_t GetBusUniqStops(const std::string& name); // Количество уникальных остановок

    std::unordered_map<std::string, Stop::Stop>::iterator GetKeyStop(const std::string& name); // Получение ключа остановки

    std::unordered_map<std::string, Bus::Bus>::iterator GetKeyBus(const std::string& name); // Получение ключа автобуса

    bool CheckBus(const std::string& name) const; // Наличие остановки

    bool CheckStop(const std::string& name) const; // Наличие автобуса

    double GetRoadDistance(const std::string& name); // Подсчет дорожного расстояния

    double GetCurvatureRoute(const std::string& name); // Получения кривизны маршрута

    std::set<std::string_view> GetBusesPassingTheStop(const std::string& name);

private:
    std::unordered_map<std::string, Stop::Stop> stops_; // Остановки
    std::unordered_map<std::string, Bus::Bus> buses_; // Автобусы
    std::unordered_map<std::string_view, RouteData::RouteData> route_data; // Информация о маршруте
    std::unordered_map<std::pair<std::string_view, std::string_view>, uint32_t, Hash::Hash> distance_between_stops_; // Дистанция между остановками
    std::unordered_map<std::string_view, std::set<std::string_view>> buses_passing_through_the_stop_; // Автобусы, проходящие через остановку

private:
    double GetDistanceToStop(std::string_view name, std::string_view stop) ; // Получение расстояния до остановки
};

} // TransportCatalogue





