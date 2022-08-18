#include "json_reader.h"

#include <sstream>

using namespace std;

namespace JsonReader {

void JsonReader::ReadInputData(istream& input) {
    json_data_ = std::get<Json::Dict>(Json::Load(input).GetRoot().GetValue());
    GetStopsFromData();
    AppendStopsToTransportCatalogue();
    GetBusesFromData();
    AppendBusesToTransportCatalogue();
    GetRenderSettingsFromData();
    GetRequestsFromData();
}

void JsonReader::GetBusesFromData() {
    for (const Json::Node& data : json_data_.at("base_requests"s).AsArray()) {
        const Json::Dict& dict = data.AsMap();
        if (dict.at("type").AsString() == "Bus") {
            TransportCatalogue::Bus::Bus bus;
            bus.name = dict.at("name").AsString();
            bus.is_circle = dict.at("is_roundtrip").AsBool();
            const Json::Array& stops = dict.at("stops").AsArray();
            for (const auto& stop : stops) {
                bus.route.push_back(transport_catalogue_.GetStop(stop.AsString()));
            }
            deque_buses_.push_back(move(bus));
        }
    }
}

void JsonReader::GetStopsFromData() {
    for (const Json::Node& data : json_data_.at("base_requests"s).AsArray()) {
        const Json::Dict& dict = data.AsMap();
        if (dict.at("type").AsString() == "Stop") {
            TransportCatalogue::Stop::Stop stop;
            stop.name = dict.at("name").AsString();
            stop.сoordinates = {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
            const Json::Dict& dict_road_distances = dict.at("road_distances").AsMap();
            for (const auto& [key, value] : dict_road_distances) {
                DistanceBetweenStops::DistanceBetweenStops distance_between_stop;
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
    Json::Dict map = json_data_.at("render_settings"s).AsMap();
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

vector<Svg::Color> JsonReader::GetColorFromArray(const Json::Array& array) {
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

void JsonReader::GetRequestsFromData() {
    for (const Json::Node& data : json_data_.at("stat_requests"s).AsArray()) {
        const Json::Dict& dict = data.AsMap();
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

const deque<RequestData::RequestData>& JsonReader::GetRequests() const {
    return deque_requests_;
}

const RenderSettings::RenderSettings& JsonReader::GetRenderSettings() const {
    return render_settings_;
}

} // namespace JsonReader
