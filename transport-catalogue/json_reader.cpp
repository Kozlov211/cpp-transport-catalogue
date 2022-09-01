#include <sstream>

#include "json_reader.h"
#include "map_renderer.h"
#include "json_builder.h"

using namespace std;

namespace json_reader {

using namespace distance_between_stops;
using namespace request_data;
using namespace json;

void JsonReader::ReadInputData(istream& input) {
    json_data_ = std::get<Dict>(Load(input).GetRoot().GetValue());
    GetStopsFromData();
    AppendStopsToTransportCatalogue();
    GetBusesFromData();
    AppendBusesToTransportCatalogue();
    GetRenderSettingsFromData();
    GetRequestsFromData();
}

void JsonReader::GetBusesFromData() {
    for (const Node& data : json_data_.at("base_requests"s).AsArray()) {
        const Dict& dict = data.AsDict();
        if (dict.at("type").AsString() == "Bus") {
            transport_catalogue::bus::Bus bus;
            bus.name = dict.at("name").AsString();
            bus.is_circle = dict.at("is_roundtrip").AsBool();
            const Array& stops = dict.at("stops").AsArray();
            for (const auto& stop : stops) {
                bus.route.push_back(transport_catalogue_.GetStop(stop.AsString()));
            }
            deque_buses_.push_back(move(bus));
        }
    }
}

void JsonReader::GetStopsFromData() {
    for (const Node& data : json_data_.at("base_requests"s).AsArray()) {
        const Dict& dict = data.AsDict();
        if (dict.at("type").AsString() == "Stop") {
            transport_catalogue::stop::Stop stop;
            stop.name = dict.at("name").AsString();
            stop.сoordinates = {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
            const Dict& dict_road_distances = dict.at("road_distances").AsDict();
            for (const auto& [key, value] : dict_road_distances) {
                DistanceBetweenStops distance_between_stop;
                distance_between_stop.name = stop.name;
                distance_between_stop.stop = key;
                distance_between_stop.road_distances = value.AsInt();
                deque_distance_between_stops_.push_back(move(distance_between_stop));
            }
            deque_stops_.push_back(move(stop));
        }
    }

}

void JsonReader::GetRenderSettingsFromData() {
    Dict map = json_data_.at("render_settings"s).AsDict();
    render_settings_.width = map.at("width").AsDouble();
    render_settings_.height = map.at("height").AsDouble();
    render_settings_.padding = map.at("padding").AsDouble();
    render_settings_.line_width = map.at("line_width").AsDouble();
    render_settings_.stop_radius = map.at("stop_radius").AsDouble();
    render_settings_.bus_label_font_size = map.at("bus_label_font_size").AsInt();
    for (auto& value :  map.at("bus_label_offset").AsArray()) {
        render_settings_.bus_label_offset.push_back(value.AsDouble());
    }
    render_settings_.stop_label_font_size = map.at("stop_label_font_size").AsDouble();
    for (auto& value :  map.at("stop_label_offset").AsArray()) {
        render_settings_.stop_label_offset.push_back(value.AsDouble());
    }
    render_settings_.underlayer_color = GetColor(map.at("underlayer_color"));
    render_settings_.underlayer_width = map.at("underlayer_width").AsDouble();
    render_settings_.color_palette = GetColorFromArray(map.at("color_palette").AsArray());

}

vector<Svg::Color> JsonReader::GetColorFromArray(const Array& array) {
    vector<Svg::Color> result;
    for (auto& value : array) {
        result.push_back(GetColor(value));
    }

    return result;
}

void JsonReader::AppendStopsToTransportCatalogue() {
    for (auto& stop : deque_stops_) {
        transport_catalogue_.AppendStop(stop.name, &stop);
    }
    for (auto& stop : deque_distance_between_stops_) {
        transport_catalogue_.AppendDistanceToStop(transport_catalogue_.GetStop(stop.name), transport_catalogue_.GetStop(stop.stop), stop.road_distances);
    }
}

void JsonReader::AppendBusesToTransportCatalogue() {
    for (auto& bus : deque_buses_) {
        transport_catalogue_.AppendBus(bus.name, &bus);
        for (auto* stop : bus.route) {
            transport_catalogue_.AppendBusToStop(stop, &bus);
        }
    }
}

std::string JsonReader::GetResponseToRequest(map_renderer::MapRenderer& map_render) {
    Array response;
    Builder builder;
    builder.StartArray();
    std::ostringstream out;
    for (auto& request: deque_requests_) {
        if (request.type == "Bus") {
            GetInformationAboutBus(builder, request);
        } else if (request.type == "Stop" ) {
            GetInformationAboutStop(builder, request);
        } else {
            GetInformationAboutMap(builder, request, map_render);
        }
    }
    Print(Document{builder.EndArray().Build()}, out);
    return out.str();
}

void JsonReader::GetInformationAboutStop(Builder& builder, const RequestData& request) {
    if (transport_catalogue_.CheckStop(request.name)) {
        const transport_catalogue::stop::Stop* stop = transport_catalogue_.GetStop(request.name);
        builder.StartDict().Key("request_id"s).Value(request.id);
        builder.Key("buses"s).StartArray();
        for (const auto& bus : transport_catalogue_.GetBusesPassingTheStop(stop->name)) {
            builder.Value(string(bus));
        }
        builder.EndArray().EndDict();
    } else {
        builder.StartDict().Key("error_message"s).Value("not found"s).Key("request_id"s).Value(request.id).EndDict();
    }
}

void JsonReader::GetInformationAboutBus(Builder& builder, const RequestData& request) {
    if (transport_catalogue_.CheckBus(request.name)) {
        transport_catalogue::bus::Bus* bus = transport_catalogue_.GetBus(request.name);
        builder.StartDict().Key("request_id"s).Value(request.id).Key("curvature"s).Value(transport_catalogue_.GetCurvatureRoute(bus))
                .Key("route_length"s).Value(transport_catalogue_.GetRoadLength(bus)).Key("stop_count"s).Value(static_cast<int>(transport_catalogue_.GetNumberStopsOnTheRoute(bus)))
                .Key("unique_stop_count"s).Value(static_cast<int>(transport_catalogue_.GetNumberUniqueStopsOnTheRoute(bus))).EndDict();
    } else {
        builder.StartDict().Key("error_message"s).Value("not found"s).Key("request_id"s).Value(request.id).EndDict();
    }
}

void JsonReader::GetInformationAboutMap(Builder& builder, const RequestData& request, map_renderer::MapRenderer& map_render) {
    builder.StartDict().Key("request_id"s).Value(request.id).Key("map"s)
            .Value(map_render.GetMapAsString(render_settings_)).EndDict();
}

void JsonReader::GetRequestsFromData() {
    for (const Node& data : json_data_.at("stat_requests"s).AsArray()) {
        const Dict& dict = data.AsDict();
        if (dict.at("type").AsString() == "Map") {
            deque_requests_.push_back({(dict.at("id").AsInt()),
                                       dict.at("type").AsString(),
                                       ""});
        } else {
            deque_requests_.push_back({(dict.at("id").AsInt()),
                                       dict.at("type").AsString(),
                                       dict.at("name").AsString()});
        }
    }
}

} // namespace json_reader
