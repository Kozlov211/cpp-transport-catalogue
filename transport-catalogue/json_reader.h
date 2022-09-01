#pragma once

#include <deque>
#include <unordered_map>
#include <sstream>
#include <string>

#include "json.h"
#include "svg.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "json_builder.h"

namespace json_reader {

namespace distance_between_stops {

struct DistanceBetweenStops {
    std::string name;
    std::string stop;
    uint32_t road_distances;
};

} // namespace distance_between_stops

namespace request_data {

struct RequestData {
    int id;
    std::string type;
    std::string name;
};

} // namespace request_data

using namespace distance_between_stops;
using namespace request_data;
using namespace json;


class JsonReader {
public:
    JsonReader(transport_catalogue::TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue) {};

    void ReadInputData(std::istream& input);

    std::string GetResponseToRequest(map_renderer::MapRenderer& map_render);

private:
    transport_catalogue::TransportCatalogue& transport_catalogue_;
    Dict json_data_;
    std::deque<transport_catalogue::bus::Bus> deque_buses_;
    std::deque<transport_catalogue::stop::Stop> deque_stops_;
    std::deque<DistanceBetweenStops> deque_distance_between_stops_;
    std::deque<RequestData> deque_requests_;
    map_renderer::render_settings::RenderSettings render_settings_;

private:
    void GetBusesFromData();

    void GetStopsFromData();

    void GetRenderSettingsFromData();

    void AppendStopsToTransportCatalogue();

    void AppendBusesToTransportCatalogue();

    void GetInformationAboutStop(Builder& builder, const RequestData& request);

    void GetInformationAboutBus(Builder& builder, const RequestData& request);

    void GetInformationAboutMap(Builder& builder, const RequestData& request, map_renderer::MapRenderer& map_render);

    void GetRequestsFromData();

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
