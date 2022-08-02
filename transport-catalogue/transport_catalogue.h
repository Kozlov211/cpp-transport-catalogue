#pragma once
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>

#include "geo.h"

namespace TransportCatalogue {

namespace Stop {

struct Stop {
    Coordinates сoordinates;
    std::set<std::string_view> buses;
    std::unordered_map<std::string_view, double> distance_to_stop;
};

} // namespace Stop

namespace Bus {

struct Bus {
    bool is_circle;
    std::vector<std::string_view> route;
};

} // namespace Bus


class TransportCatalogue {
public:
    TransportCatalogue() = default;

    void AddStop(const std::string& name, Coordinates сoordinates,const bool is_empty); // Добавление остановки

    void AddBusToStop (const std::string& name, std::string_view bus); // Добавление автобуса к остановки

    void AddDistanceToStop (const std::string& name, std::string_view stop, double distance);

    void AddBusRoute(const std::string& name, const bool is_circle, std::vector<std::string_view> route); // Добавление маршрута

    Stop::Stop GetStop(const std::string& name); // Получение остановки

    std::vector<std::string_view> GetBusRoute(const std::string& name); // Получение маршрута автобуса

    double CountGeographicDistance(const std::string& name); // Подсчет географического расстояния

    size_t BusStopCount(const std::string& name); // Количество остановок на маршруте

    size_t BusUniqStopCount(const std::string& name); // Количество уникальных остановок

    std::unordered_map<std::string, Stop::Stop>::iterator GetKeyStop(const std::string& name); // Получение ключа остановки

    std::unordered_map<std::string, Bus::Bus>::iterator GetKeyBus(const std::string& name); // Получение ключа автобуса

    bool CheckBus(const std::string& name) const; // Наличие остановки

    bool CheckStop(const std::string& name) const; // Наличие автобуса

    double GetDistanceToStop(const std::string& name, std::string_view stop) ; // Получение расстояния до остановки

    double CountRoadDistance(const std::string& name); // Подсчет дорожного расстояния

    double GetCurvatureRoute(const std::string& name); // Получения кривизны маршрута

private:
    std::unordered_map<std::string, Stop::Stop> stops_; // Остановки
    std::unordered_map<std::string, Bus::Bus> buses_; // Автобусы
};

} // TransportCatalogue



// место для вашего кода
