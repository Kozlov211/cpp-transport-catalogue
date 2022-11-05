#pragma once

#include <deque>
#include <sstream>
#include <string>
#include <variant>
#include <unordered_map>

#include "map_renderer.h"
#include "json_builder.h"
#include "json.h"
#include "svg.h"
#include "transport_catalogue.h"
#include "transport_router.h"

namespace json_reader {

class JsonReader {
public:
    JsonReader(std::istream& input);

    void AppendDataToTransportCatalogue(transport_catalogue::TransportCatalogue& transport_catalogue);

    domain::RenderSettings GetRenderSettings();

    domain::RoutingSettings GetRoutingSettings();

    std::deque<domain::RequestData> GetRequestData();

    domain::SerializationSettings GetSerializationSettings();

    std::string GetResponseToRequest(const transport_catalogue::TransportCatalogue& transport_catalogue, const map_renderer::MapRenderer& map_render) const;

private:
    json::Dict json_data_;
    std::deque<domain::DistanceBetweenStops> deque_distance_between_stops_;

private:
    void AppendBusesToTransportCatalogue(transport_catalogue::TransportCatalogue& transport_catalogue);

    void AppendStopsToTransportCatalogue(transport_catalogue::TransportCatalogue& transport_catalogue);

    void AppendDistanceBetweenStopsToTransportCatalogue(transport_catalogue::TransportCatalogue& transport_catalogue);

    void AppendRenderSettingsToTransportCatalogue(transport_catalogue::TransportCatalogue& transport_catalogue);

    void AppendRoutingSettingsToTransportCatalogue(transport_catalogue::TransportCatalogue& transport_catalogue);

    domain::RenderSettings GetRenderSettingsFromJson();

    domain::RoutingSettings GetRoutingSettingsFromJson();

    domain::SerializationSettings GetSerializationSettingsFromJson();

    std::deque<domain::RequestData> GetRequestDataFromJson();

    std::vector<svg::Color> GetColorFromArray(const json::Array& array);

    template <typename Type>
    svg::Color GetColor(const Type& color);
};

template <typename Type>
svg::Color JsonReader::GetColor(const Type& color) {
    if (color.IsString()) {
        return {color.AsString()};
    }
    if (color.IsArray() && color.AsArray().size() == 3) {
        std::vector<uint8_t> color_parts;
        for (auto& value : color.AsArray()) {
            color_parts.push_back(value.AsInt());
        }
        svg::Rgb rgb{color_parts[0], color_parts[1], color_parts[2]};
        return {rgb};
    }
    std::vector<double> color_parts;
    for (auto& value : color.AsArray()) {
        color_parts.push_back(value.AsDouble());
    }
    svg::Rgba rgba {static_cast<uint8_t>(color_parts[0]),
                    static_cast<uint8_t>(color_parts[1]),
                    static_cast<uint8_t>(color_parts[2]),
                    static_cast<double>(color_parts[3])};
    return {rgba};
}

} // namespace json_reader
