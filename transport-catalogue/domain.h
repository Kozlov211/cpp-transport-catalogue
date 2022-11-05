#pragma once

#include "geo.h"
#include "svg.h"
#include "graph.h"

#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace domain {

struct DistanceBetweenStops {
    std::string from_stop;
    std::string to_stop;
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

struct Stop {
    std::string name;
    coordinates::Coordinates сoordinates;
    size_t index;
};

struct Bus {
    std::string name;
    std::vector<Stop*> route;
    bool is_circle;
};

struct RouteData {
    std::optional<size_t> stops;
    std::optional<size_t> uniq_stops;
    std::optional<double> geographic_legth;
    std::optional<double> road_length;
    std::optional<double> curvature_route;
};

using RequestData = std::variant<RequestMap, RequestBusOrStop, RequestRoute>;

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
    svg::Color underlayer_color;
    double underlayer_width;
    std::vector<svg::Color> color_palette;
};

struct RoutingSettings {
    uint32_t bus_wait_time;
    uint32_t bus_velocity;
};

struct StopPairVertexId {
    graph::VertexId bus_wait_begin;
    graph::VertexId bus_wait_end;
};

struct WaitEdgeInfo {
    std::string_view stop_name;
    double time = 0;
};

struct BusEdgeInfo {
    std::string_view bus_name;
    size_t span_count = 0;
    double time = 0;
};

using EdgeInfo = std::variant<WaitEdgeInfo, BusEdgeInfo>;

struct RouteInfo {
    double total_time = 0;
    std::vector<EdgeInfo> edges;
};

struct SerializationSettings {
    std::string name_file;
};

}
