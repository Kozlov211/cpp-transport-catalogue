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
    RequestHandler(transport_catalogue::TransportCatalogue& transport_catalogue, map_renderer::MapRenderer& map_render)
        : transport_catalogue_( transport_catalogue), map_render_(map_render) {}

    void AppendBusesToMapRender();

    void AppendGeoCoordinatesToMapRender();

    std::string GetResponseToRequest(json_reader::JsonReader& json_reader);

private:
    transport_catalogue::TransportCatalogue& transport_catalogue_;
    map_renderer::MapRenderer& map_render_;
    std::vector<coordinates::Coordinates> geo_coordinates;

private:
    void ReadGeoCoordinatesFromStops();

    void ReadNameBusesFromBuses();
};

} // namespace RequestHandler
