#include <sstream>

#include "request_handler.h"

using namespace std;

namespace RequestHandler {

void RequestHandler::AppendBusesToMapRender() {
    map_render_.AppendBuses(transport_catalogue_.GetBuses());
}

void RequestHandler::AppendGeoCoordinatesToMapRender() {
    ReadGeoCoordinatesFromStops();
    map_render_.AppendCoordinates(move(geo_coordinates));
}

void RequestHandler::ReadGeoCoordinatesFromStops() {
    const std::unordered_map<std::string_view, transport_catalogue::stop::Stop*>& stops = transport_catalogue_.GetStops();
    for (const auto& [key, value] : stops) {
        if (!transport_catalogue_.GetBusesPassingTheStop(key).empty()){
            geo_coordinates.push_back(value->сoordinates);
        }
    }
}

std::string RequestHandler::GetResponseToRequest(json_reader::JsonReader& json_reader) {
    return json_reader.GetResponseToRequest(map_render_);
}

} // namespace RequestHandler
