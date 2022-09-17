#include <sstream>

#include "json_reader.h"
#include "map_renderer.h"
#include "json_builder.h"


using namespace std;

namespace json_reader {

using namespace json;

void JsonReader::ReadJson(istream& input) {
    json_data_ = std::get<Dict>(Load(input).GetRoot().GetValue());
    GetStopsFromJson();
    AppendStopsToTransportCatalogue();
    AppendDistanceBetweenStopsToTransportCatalogue();
    GetBusesFromJson();
    AppendBusesToTransportCatalogue();
    GetRenderSettingsFromJson();
    GetRequestsFromJson();
    GetRoutingSettingsFromJson();
}

void JsonReader::GetBusesFromJson() {
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

void JsonReader::GetStopsFromJson() {
    size_t index = 0;
    for (const Node& data : json_data_.at("base_requests"s).AsArray()) {
        const Dict& dict = data.AsDict();
        if (dict.at("type").AsString() == "Stop") {
            transport_catalogue::stop::Stop stop;
            stop.name = dict.at("name").AsString();
            stop.сoordinates = {dict.at("latitude").AsDouble(), dict.at("longitude").AsDouble()};
            stop.index = index++;
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

void JsonReader::GetRenderSettingsFromJson() {
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

void JsonReader::GetRequestsFromJson() {
    for (const Node& data : json_data_.at("stat_requests"s).AsArray()) {
        const Dict& dict = data.AsDict();
        if (dict.at("type").AsString() == "Map"s) {
            RequestMap map {dict.at("id"s).AsInt()};
            deque_requests_.push_back(move(map));
        } else if (dict.at("type"s).AsString() == "Route"s) {
            RequestRoute route {(dict.at("id"s).AsInt()), dict.at("from"s).AsString(), dict.at("to"s).AsString()};
            deque_requests_.push_back(move(route));
        } else {
            RequestBusOrStop bus_or_stop {(dict.at("id"s).AsInt()), dict.at("type"s).AsString(), dict.at("name"s).AsString()};
            deque_requests_.push_back(move(bus_or_stop));
        }
    }
}

void JsonReader::GetRoutingSettingsFromJson() {
    const Dict& dict = json_data_.at("routing_settings"s).AsDict();
    routing_settings_.bus_velocity = dict.at("bus_velocity"s).AsInt();
    routing_settings_.bus_wait_time = dict.at("bus_wait_time"s).AsInt();
}

std::string JsonReader::GetResponseToRequest(map_renderer::MapRenderer& map_render) {
    Array response;
    Builder builder;
    builder.StartArray();
    std::ostringstream out;
    transport_router::TransportRouter transport_router(routing_settings_);
    transport_router.BuildRouter(transport_catalogue_);
    for (auto& request: deque_requests_) {
        if (holds_alternative<RequestMap>(request)) {
            BuildJsonMap(builder, std::get<RequestMap>(request), map_render);
        } else if (holds_alternative<RequestRoute>(request)) {
            BuildJsonRoute(builder, std::get<RequestRoute>(request), transport_router);
        } else {
            RequestBusOrStop request_bus_or_stop = std::get<RequestBusOrStop>(request);
            if (request_bus_or_stop.type == "Bus") {
                BuildJsonBus(builder, request_bus_or_stop);
            } else if (request_bus_or_stop.type == "Stop" ) {
                BuildJsonStop(builder, request_bus_or_stop);
            }
        }
    }
    Print(Document{builder.EndArray().Build()}, out);
    return out.str();
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
}

void JsonReader::AppendDistanceBetweenStopsToTransportCatalogue() {
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

void JsonReader::BuildJsonStop(Builder& builder, const RequestBusOrStop& request) {
    if (transport_catalogue_.CheckStop(request.name)) {
        const transport_catalogue::stop::Stop* stop = transport_catalogue_.GetStop(request.name);
        builder.StartDict().Key("request_id"s).Value(request.id);
        builder.Key("buses"s).StartArray();
        for (const auto& bus : transport_catalogue_.GetBusesPassingTheStop(stop->name)) {
            builder.Value(string(bus));
        }
        builder.EndArray().EndDict();
    } else {
        BuildJsonErrorMessage(builder, request.id);
    }
}

void JsonReader::BuildJsonBus(Builder& builder, const RequestBusOrStop& request) {
    if (transport_catalogue_.CheckBus(request.name)) {
        transport_catalogue::bus::Bus* bus = transport_catalogue_.GetBus(request.name);
        builder.StartDict().Key("request_id"s).Value(request.id).Key("curvature"s).Value(transport_catalogue_.GetCurvatureRoute(bus))
                .Key("route_length"s).Value(transport_catalogue_.GetRoadLength(bus)).Key("stop_count"s).Value(static_cast<int>(transport_catalogue_.GetNumberStopsOnTheRoute(bus)))
                .Key("unique_stop_count"s).Value(static_cast<int>(transport_catalogue_.GetNumberUniqueStopsOnTheRoute(bus))).EndDict();
    } else {
        BuildJsonErrorMessage(builder, request.id);
    }
}

void JsonReader::BuildJsonMap(Builder& builder, const RequestMap& request, map_renderer::MapRenderer& map_render) {
    builder.StartDict().Key("request_id"s).Value(request.id).Key("map"s)
            .Value(map_render.GetMapAsString(render_settings_)).EndDict();
}

void JsonReader::BuildJsonRoute(Builder& builder, const RequestRoute& request, const transport_router::TransportRouter& transport_router) {
    if (transport_catalogue_.CheckStop(request.from) && transport_catalogue_.CheckStop(request.to)) {
        std::optional<transport_router::StopPairVertexId> pair_vertex_id_stop_from = transport_router.GetPairVertexId(transport_catalogue_.GetStop(request.from));
        std::optional<transport_router::StopPairVertexId> pair_vertex_id_stop_to = transport_router.GetPairVertexId(transport_catalogue_.GetStop(request.to));
        std::optional<transport_router::RouteInfo> route_info = transport_router.GetRouteInfo(pair_vertex_id_stop_from->bus_wait_begin, pair_vertex_id_stop_to->bus_wait_begin);
        if (route_info) {
            builder.StartDict().Key("request_id"s).Value(request.id).Key("total_time"s).Value(route_info->total_time).Key("items"s).StartArray();
            for (auto& info : route_info->edges) {
                if (info.index() == 1) {
                    BuildJsonBusEdge(builder, get<transport_router::BusEdgeInfo>(info));
                } else {
                    BuildJsonWaitEdge(builder, get<transport_router::WaitEdgeInfo>(info));
                }
            }
            builder.EndArray().EndDict();
        } else {
            BuildJsonErrorMessage(builder, request.id);
        }

    } else {
        BuildJsonErrorMessage(builder, request.id);
    }
}

void JsonReader::BuildJsonBusEdge(Builder& builder, const transport_router::BusEdgeInfo& bus_edge_info) {
    builder.StartDict().Key("type"s).Value("Bus"s)
            .Key("bus"s).Value(string(bus_edge_info.bus_name))
            .Key("span_count"s).Value(static_cast<int>(bus_edge_info.span_count))
            .Key("time"s).Value(bus_edge_info.time).EndDict();
}

void JsonReader::BuildJsonWaitEdge(Builder& builder, const transport_router::WaitEdgeInfo& wait_edge_info) {
    builder.StartDict().Key("type"s).Value("Wait"s)
            .Key("stop_name"s).Value(string(wait_edge_info.stop_name))
            .Key("time"s).Value(wait_edge_info.time).EndDict();
}

void JsonReader::BuildJsonErrorMessage(Builder& builder, const int request_id) {
    builder.StartDict().Key("error_message"s).Value("not found"s).Key("request_id"s).Value(request_id).EndDict();
}

} // namespace json_reader
