#pragma once

#include <set>

#include "json_reader.h"
#include "map_renderer.h"
#include "geo.h"
#include "transport_catalogue.h"


namespace RequestHandler {

class RequestHandler {
public:
    RequestHandler(TransportCatalogue::TransportCatalogue& transport_catalogue, MapRenderer::MapRenderer& map_render)
        : transport_catalogue_( transport_catalogue), map_render_(map_render) {}

    void AppendRequests(const JsonReader::JsonReader& json_reader);

    void AppendBusesToMapRender();

    void AppendGeoCoordinatesToMapRender();

    std::string GetResponseToRequest();

    const std::vector<std::string_view>& GetNameBuses();

private:
    TransportCatalogue::TransportCatalogue& transport_catalogue_;
    MapRenderer::MapRenderer& map_render_;
    std::deque<JsonReader::RequestData::RequestData> deque_requests;
    std::unordered_map<std::string, Json::Node> render_settings_;
    std::vector<Coordinates> geo_coordinates;
    std::vector<std::string_view> name_of_buses;

private:
    Json::Dict GetInformationAboutStop(const JsonReader::RequestData::RequestData& request);

    Json::Dict GetInformationAboutBus(const JsonReader::RequestData::RequestData& request);

    Json::Dict GetInformationAboutMap(const JsonReader::RequestData::RequestData& request);

    void ReadGeoCoordinatesFromStops();

    void ReadNameBusesFromBuses();
};

} // namespace RequestHandler
