#pragma once

#include <deque>
#include <unordered_map>
#include <sstream>

#include "json.h"
#include "svg.h"
#include "transport_catalogue.h"


namespace JsonReader {

namespace RenderSettings {

struct RenderSettings {
    double width;
    double height;
    double padding;
    double line_width;
    double stop_radius;
    uint32_t bus_label_font_size;
    std::vector<double> bus_label_offset;
    uint32_t stop_label_font_size;
    std::vector<double> stop_label_offset;
    Svg::Color underlayer_color;
    double underlayer_width;
    std::vector<Svg::Color> color_palette;
};

} // namespace RenderSettings

namespace DistanceBetweenStops {

struct DistanceBetweenStops {
    std::string name;
    std::string stop;
    uint32_t road_distances;
};

} // namespace DistanceBetweenStops

namespace RequestData {

struct RequestData {
    int id;
    std::string type;
    std::string name;
};

} // namespace RenderDataMap


class JsonReader {
public:
    JsonReader(TransportCatalogue::TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue) {};

    void ReadInputData(std::istream& input);

    std::string GetResponseToRequest();

    const std::deque<RequestData::RequestData>& GetRequests() const;

    const RenderSettings::RenderSettings& GetRenderSettings() const;

private:
    TransportCatalogue::TransportCatalogue& transport_catalogue_;
    Json::Dict json_data_;
    std::deque<TransportCatalogue::Bus::Bus> deque_buses_;
    std::deque<TransportCatalogue::Stop::Stop> deque_stops_;
    std::deque<DistanceBetweenStops::DistanceBetweenStops> deque_distance_between_stops_;
    std::deque<RequestData::RequestData> deque_requests_;
    RenderSettings::RenderSettings render_settings_;
private:

    void GetBusesFromData();

    void GetStopsFromData();

    void GetRenderSettingsFromData();

    void AppendStopsToTransportCatalogue();

    void AppendBusesToTransportCatalogue();

    void GetRequestsFromData();

    std::vector<Svg::Color> GetColorFromArray(const Json::Array& array);

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

} // namespace JsonReader
