#include <sstream>

#include "domain.h"
#include "json_reader.h"
#include "map_renderer.h"
#include "json_builder.h"

namespace json_reader {
using namespace std;
using namespace json;
using namespace domain;
using namespace transport_catalogue;
using namespace transport_router;
using namespace map_renderer;

JsonReader::JsonReader(istream& input) {
    json_data_ = std::get<Dict>(Load(input).GetRoot().GetValue());
}

void JsonReader::AppendDataToTransportCatalogue(TransportCatalogue& transport_catalogue) {
    AppendStopsToTransportCatalogue(transport_catalogue);
    AppendDistanceBetweenStopsToTransportCatalogue(transport_catalogue);
    AppendBusesToTransportCatalogue(transport_catalogue);
}

void JsonReader::AppendStopsToTransportCatalogue(TransportCatalogue& transport_catalogue_) {
    size_t index = 0;
    for (const Node& data : json_data_.at("base_requests"s).AsArray()) {
        const Dict& dict = data.AsDict();
        if (dict.at("type").AsString() == "Stop") {
            domain::Stop stop;
            stop.name = dict.at("name").AsString();
            stop.сoordinates = {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
            stop.index = index++;
            const Dict& dict_road_distances = dict.at("road_distances").AsDict();
            for (const auto& [key, value] : dict_road_distances) {
                DistanceBetweenStops distance_between_stop;
                distance_between_stop.from_stop = stop.name;
                distance_between_stop.to_stop = key;
                distance_between_stop.road_distances = value.AsInt();
                deque_distance_between_stops_.push_back(move(distance_between_stop));
            }
            transport_catalogue_.AppendStop(move(stop));
        }
    }
}

void JsonReader::AppendDistanceBetweenStopsToTransportCatalogue(TransportCatalogue& transport_catalogue_) {
    for (auto& stop : deque_distance_between_stops_) {
        transport_catalogue_.AppendDistanceToStop(transport_catalogue_.GetStop(stop.from_stop), transport_catalogue_.GetStop(stop.to_stop), stop.road_distances);
    }
}

void JsonReader::AppendBusesToTransportCatalogue(TransportCatalogue& transport_catalogue_) {
    for (const Node& data : json_data_.at("base_requests"s).AsArray()) {
        const Dict& dict = data.AsDict();
        if (dict.at("type").AsString() == "Bus") {
            domain::Bus bus;
            bus.name = dict.at("name").AsString();
            bus.is_circle = dict.at("is_roundtrip").AsBool();
            const Array& stops = dict.at("stops").AsArray();
            for (const auto& stop : stops) {
                bus.route.push_back(transport_catalogue_.GetStop(stop.AsString()));
            }
            transport_catalogue_.AppendBus(move(bus));
        }
    }
}

RenderSettings JsonReader::GetRenderSettings() {
    return GetRenderSettingsFromJson();
}

RoutingSettings JsonReader::GetRoutingSettings() {
    return GetRoutingSettingsFromJson();
}

SerializationSettings JsonReader::GetSerializationSettings() {
    return GetSerializationSettingsFromJson();
}

RenderSettings JsonReader::GetRenderSettingsFromJson() {
    RenderSettings render_settings;
    Dict map = json_data_.at("render_settings"s).AsDict();
    render_settings.width = map.at("width").AsDouble();
    render_settings.height = map.at("height").AsDouble();
    render_settings.padding = map.at("padding").AsDouble();
    render_settings.line_width = map.at("line_width").AsDouble();
    render_settings.stop_radius = map.at("stop_radius").AsDouble();
    render_settings.bus_label_font_size = map.at("bus_label_font_size").AsInt();
    for (auto& value : map.at("bus_label_offset").AsArray()) {
        render_settings.bus_label_offset.push_back(value.AsDouble());
    }
    render_settings.stop_label_font_size = map.at("stop_label_font_size").AsDouble();
    for (auto& value : map.at("stop_label_offset").AsArray()) {
        render_settings.stop_label_offset.push_back(value.AsDouble());
    }
    render_settings.underlayer_color = GetColor(map.at("underlayer_color"));
    render_settings.underlayer_width = map.at("underlayer_width").AsDouble();
    render_settings.color_palette = GetColorFromArray(map.at("color_palette").AsArray());
    return render_settings;
}

RoutingSettings JsonReader::GetRoutingSettingsFromJson() {
    const Dict& dict = json_data_.at("routing_settings"s).AsDict();
    RoutingSettings routing_settings_ = {static_cast<uint32_t>(dict.at("bus_wait_time"s).AsInt()),
                                         static_cast<uint32_t>(dict.at("bus_velocity"s).AsInt())};
    return routing_settings_;
}

SerializationSettings JsonReader::GetSerializationSettingsFromJson() {
    const Dict& dict = json_data_.at("serialization_settings"s).AsDict();
    SerializationSettings serialization_settings = {move(dict.at("file"s).AsString())};
    return serialization_settings;
}

std::deque<RequestData> JsonReader::GetRequestData() {
    return GetRequestDataFromJson();
}

std::deque<RequestData> JsonReader::GetRequestDataFromJson() {
    deque<RequestData> requests_;
    for (const Node& data : json_data_.at("stat_requests"s).AsArray()) {
        const Dict& dict = data.AsDict();
        if (dict.at("type").AsString() == "Map"s) {
            domain::RequestMap map {dict.at("id"s).AsInt()};
            requests_.push_back(move(map));
        } else if (dict.at("type"s).AsString() == "Route"s) {
            domain::RequestRoute route {(dict.at("id"s).AsInt()), dict.at("from"s).AsString(), dict.at("to"s).AsString()};
            requests_.push_back(move(route));
        } else {
            domain::RequestBusOrStop bus_or_stop {(dict.at("id"s).AsInt()), dict.at("type"s).AsString(), dict.at("name"s).AsString()};
            requests_.push_back(move(bus_or_stop));
        }
    }
    return requests_;
}

vector<svg::Color> JsonReader::GetColorFromArray(const Array& array) {
    vector<svg::Color> result;
    for (auto& value : array) {
        result.push_back(GetColor(value));
    }
    return result;
}


} // namespace json_reader
