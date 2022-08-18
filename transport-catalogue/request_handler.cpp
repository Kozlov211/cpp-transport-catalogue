#include <sstream>

#include "request_handler.h"

using namespace std;

namespace RequestHandler {

void RequestHandler::AppendRequests(const JsonReader::JsonReader& json_reader) {
    deque_requests = json_reader.GetRequests();
}

std::string RequestHandler::GetResponseToRequest() {
    Json::Array response;
    std::ostringstream out;
    for (auto& request: deque_requests) {
        if (request.type == "Bus") {
            response.push_back(GetInformationAboutBus(request));
        } else if (request.type == "Stop" ) {
            response.push_back(GetInformationAboutStop(request));
        } else {
            response.push_back(GetInformationAboutMap(request));
        }
    }
    Json::Print(Json::Document{response}, out);
    return out.str();
}

void RequestHandler::AppendBusesToMapRender() {
    map_render_.AppendBuses(transport_catalogue_.GetBuses());
}

Json::Dict RequestHandler::GetInformationAboutStop(const JsonReader::RequestData::RequestData& request) {
    Json::Dict information;
    if (transport_catalogue_.CheckStop(request.name)) {
        const TransportCatalogue::Stop::Stop* stop = transport_catalogue_.GetStop(request.name);
        information["request_id"] = request.id;
        Json::Array buses;
        for (const auto& bus : transport_catalogue_.GetBusesPassingTheStop(stop->name)) {
            buses.push_back(string(bus));
        }
        information["buses"] = buses;
        return information;
    } else {
        information["error_message"] = "not found"s;
        information["request_id"] = request.id;
        return information;
    }
}

Json::Dict RequestHandler::GetInformationAboutBus(const JsonReader::RequestData::RequestData& request) {
    Json::Dict information;
    if (transport_catalogue_.CheckBus(request.name)) {
        TransportCatalogue::Bus::Bus* bus = transport_catalogue_.GetBus(request.name);
        information["request_id"] = request.id;
        information["curvature"] = transport_catalogue_.GetCurvatureRoute(bus);
        information["route_length"] = transport_catalogue_.GetRoadLength(bus);
        information["stop_count"] = static_cast<int>(transport_catalogue_.GetNumberStopsOnTheRoute(bus));
        information["unique_stop_count"] = static_cast<int>(transport_catalogue_.GetNumberUniqueStopsOnTheRoute(bus));
        return information;
    } else {
        information["error_message"] = "not found"s;
        information["request_id"] = request.id;
        return information;
    }
}

Json::Dict RequestHandler::GetInformationAboutMap(const JsonReader::RequestData::RequestData& request) {
    Json::Dict information;
    information["request_id"] = request.id;
    information["map"] = map_render_.GetMapAsString();
    return information;
}

void RequestHandler::AppendGeoCoordinatesToMapRender() {
    ReadGeoCoordinatesFromStops();
    map_render_.AppendCoordinates(move(geo_coordinates));
}

void RequestHandler::ReadGeoCoordinatesFromStops() {
    const std::unordered_map<std::string_view, TransportCatalogue::Stop::Stop*>& stops = transport_catalogue_.GetStops();
    for (const auto& [key, value] : stops) {
        if (!transport_catalogue_.GetBusesPassingTheStop(key).empty()){
            geo_coordinates.push_back(value->сoordinates);
        }
    }
}

const std::vector<std::string_view>& RequestHandler::GetNameBuses() {
    return name_of_buses;
}

void RequestHandler::ReadNameBusesFromBuses() {
    const std::unordered_map<std::string_view, TransportCatalogue::Bus::Bus*>& buses = transport_catalogue_.GetBuses();
    for (const auto& [key, value] : buses) {
        name_of_buses.push_back(key);
    }
}

} // namespace RequestHandler
