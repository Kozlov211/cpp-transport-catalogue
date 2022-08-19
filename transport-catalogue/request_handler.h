#pragma once

#include <set>
#include <string>

#include "json_reader.h"
#include "map_renderer.h"
#include "geo.h"
#include "transport_catalogue.h"


namespace RequestHandler {

class RequestHandler {
public:
    RequestHandler(TransportCatalogue::TransportCatalogue& transport_catalogue, MapRenderer::MapRenderer& map_render)
        : transport_catalogue_( transport_catalogue), map_render_(map_render) {}

    void AppendBusesToMapRender();

    void AppendGeoCoordinatesToMapRender();

    std::string GetResponseToRequest(JsonReader::JsonReader& json_reader);

    const std::vector<std::string_view>& GetNameBuses();

private:
    TransportCatalogue::TransportCatalogue& transport_catalogue_;
    MapRenderer::MapRenderer& map_render_;
    std::vector<Coordinates> geo_coordinates;

private:
    void ReadGeoCoordinatesFromStops();

    void ReadNameBusesFromBuses();
};

} // namespace RequestHandler
