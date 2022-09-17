#pragma once

#include <deque>
#include <unordered_map>
#include <sstream>
#include <string>
#include <variant>

#include "json.h"
#include "svg.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"
#include "transport_router.h"

namespace json_reader {

struct DistanceBetweenStops {
    std::string name;
    std::string stop;
    uint32_t road_distances;
};

struct RequestMap {
    int id;
};

struct RequestBusOrStop {
    int id;
    std::string type;
    std::string name;
};

struct RequestRoute {
    int id;
    std::string from;
    std::string to;
};

using namespace json;
using RequestData = std::variant<RequestMap, RequestBusOrStop, RequestRoute>;

class JsonReader {
public:
    JsonReader(transport_catalogue::TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue) {};

    void ReadJson(std::istream& input);

    std::string GetResponseToRequest(map_renderer::MapRenderer& map_render);

    void AppendDataToTransportCatalogue();

private:
    transport_catalogue::TransportCatalogue& transport_catalogue_;
    Dict json_data_;
    std::deque<transport_catalogue::bus::Bus> deque_buses_;
    std::deque<transport_catalogue::stop::Stop> deque_stops_;
    std::deque<DistanceBetweenStops> deque_distance_between_stops_;
    std::deque<RequestData> deque_requests_;
    map_renderer::render_settings::RenderSettings render_settings_;
    transport_router::RoutingSettings routing_settings_;

private:
    void GetBusesFromJson();

    void GetStopsFromJson();

    void GetRenderSettingsFromJson();

    void GetRequestsFromJson();

    void GetRoutingSettingsFromJson();

    void AppendStopsToTransportCatalogue();

    void AppendDistanceBetweenStopsToTransportCatalogue();

    void AppendBusesToTransportCatalogue();

    void BuildJsonStop(Builder& builder, const RequestBusOrStop& request);

    void BuildJsonBus(Builder& builder, const RequestBusOrStop& request);

    void BuildJsonMap(Builder& builder, const RequestMap& request, map_renderer::MapRenderer& map_render);

    void BuildJsonRoute(Builder& builder, const RequestRoute& request, const transport_router::TransportRouter& transport_router);
    \
    void BuildJsonBusEdge(Builder& builder, const transport_router::BusEdgeInfo& bus_edge_info);

    void BuildJsonWaitEdge(Builder& builder, const transport_router::WaitEdgeInfo& wait_edge_info);

    void BuildJsonErrorMessage(Builder& builder, const int request_id);

    std::vector<Svg::Color> GetColorFromArray(const Array& array);

    template <typename Type>
    Svg::Color GetColor(const Type& color);
};

template <typename Type>
Svg::Color JsonReader::GetColor(const Type& color) {
    if (color.IsString()) {
        return {color.AsString()};
    }
    if (color.IsArray() && color.AsArray().size() == 3) {
        std::vector<uint8_t> color_parts;
        for (auto& value : color.AsArray()) {
            color_parts.push_back(value.AsInt());
        }
        Svg::Rgb rgb{color_parts[0], color_parts[1], color_parts[2]};
        return {rgb};
    }
    std::vector<double> color_parts;
    for (auto& value : color.AsArray()) {
        color_parts.push_back(value.AsDouble());
    }
    Svg::Rgba rgba {static_cast<uint8_t>(color_parts[0]),
                    static_cast<uint8_t>(color_parts[1]),
                    static_cast<uint8_t>(color_parts[2]),
                    static_cast<double>(color_parts[3])};
    return {rgba};
}

} // namespace json_reader

