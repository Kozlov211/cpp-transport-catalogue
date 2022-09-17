#pragma once

#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <unordered_map>

#include "graph.h"
#include "transport_catalogue.h"
#include "router.h"

namespace transport_router {

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
    double time = 0.;
};

struct BusEdgeInfo {
    std::string_view bus_name;
    size_t span_count = 0;
    double time = 0.;
};

using EdgeInfo = std::variant<WaitEdgeInfo, BusEdgeInfo>;

struct RouteInfo {
    double total_time = 0.;
    std::vector<EdgeInfo> edges;
};

class TransportRouter {
public:
    TransportRouter(const RoutingSettings& settings) : settings_(settings) {}

    void BuildRouter(const transport_catalogue::TransportCatalogue& transport_catalogue);

    std::optional<RouteInfo> GetRouteInfo(graph::VertexId from, graph::VertexId to) const;

    std::optional<StopPairVertexId> GetPairVertexId(transport_catalogue::stop::Stop* stop) const;

private:
    const RoutingSettings& settings_;
    graph::DirectedWeightedGraph<double> graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::unordered_map<transport_catalogue::stop::Stop*, StopPairVertexId> stop_as_pair_number_;
    std::unordered_map<graph::EdgeId, EdgeInfo> edge_id_to_type_;

private:
    void FillGraph(const transport_catalogue::TransportCatalogue& transport_catalogue);

    void FillStopIdDictionaries(const std::unordered_map<std::string_view, transport_catalogue::stop::Stop*>& stops);

    void AddWaitEdges();

    void AddBusEdges(const transport_catalogue::TransportCatalogue& transport_catalogue);

    template <typename InputIt>
    void ParseBusRouteOnEdges(InputIt range_begin, InputIt range_end, const transport_catalogue::TransportCatalogue& transport_catalogue,
                              std::string_view bus);

    graph::Edge<double> MakeBusEdge(transport_catalogue::stop::Stop* from, transport_catalogue::stop::Stop* to, const double distance) const;

    const EdgeInfo& GetEdgeInfo(graph::EdgeId id) const;
};

template <typename InputIt>
void TransportRouter::ParseBusRouteOnEdges(InputIt range_begin, InputIt range_end, const transport_catalogue::TransportCatalogue& transport_catalogue,
                          std::string_view bus) {
    for (auto stop_from = range_begin; stop_from != range_end; ++stop_from) {
        size_t distance = 0;
        size_t span_count = 0;
        for (auto stop_to = next(stop_from); stop_to != range_end; ++stop_to) {
            auto befor_stop_to = prev(stop_to);
            distance += transport_catalogue.GetDistanceToStop(*befor_stop_to, *stop_to);
            ++span_count;
            graph::EdgeId edge_id = graph_.AddEdge({MakeBusEdge(*stop_from, *stop_to, distance)});
            edge_id_to_type_[edge_id] = BusEdgeInfo{bus, span_count, graph_.GetEdge(edge_id).weight};
        }
    }
}

} // namespace transport_router
