#pragma once

#include "domain.h"
#include "graph.h"
#include "transport_catalogue.h"
#include "router.h"

#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <unordered_map>

namespace transport_router {

using namespace domain;

class TransportRouter {
public:
    TransportRouter() = default;

    TransportRouter(RoutingSettings&& settings) : settings_(std::move(settings)) {}

    void BuildRouter(const transport_catalogue::TransportCatalogue& transport_catalogue);

    std::optional<RouteInfo> GetRouteInfo(graph::VertexId from, graph::VertexId to) const;

    std::optional<StopPairVertexId> GetPairVertexId(domain::Stop* stop) const;

    const graph::DirectedWeightedGraph<double>& GetGraph() const;

    const graph::Router<double>& GetRouter() const;

    const std::unordered_map<domain::Stop*, StopPairVertexId>& GetStopAsPairNumber() const;

    const std::unordered_map<graph::EdgeId, EdgeInfo> GetEdgeidToType() const;

    const RoutingSettings& GetRoutingSettings() const;

    void SetSettings(RoutingSettings&& settings);

    void SetGraph(std::vector<graph::Edge<double>>&& edges, std::vector<graph::DirectedWeightedGraph<double>::IncidenceList>&& incidence_lists);

    void SetRouter(graph::Router<double>::RoutesInternalData&& routes_internal_data);

    void SetStopAsPairNumber(std::unordered_map<domain::Stop*, StopPairVertexId>&& stop_as_pair_number);

    void SetEdgeidToType(std::unordered_map<graph::EdgeId, EdgeInfo>&& edge_id_to_type);

private:
    RoutingSettings settings_;
    std::unique_ptr<graph::DirectedWeightedGraph<double>> graph_;
    std::unique_ptr<graph::Router<double>> router_;
    std::unordered_map<domain::Stop*, StopPairVertexId> stop_as_pair_number_;
    std::unordered_map<graph::EdgeId, EdgeInfo> edge_id_to_type_;

private:
    void FillGraph(const transport_catalogue::TransportCatalogue& transport_catalogue);

    void FillStopIdDictionaries(const std::unordered_map<std::string_view, domain::Stop*>& stops);

    void AddWaitEdges();

    void AddBusEdges(const transport_catalogue::TransportCatalogue& transport_catalogue);

    template <typename InputIt>
    void ParseBusRouteOnEdges(InputIt range_begin, InputIt range_end, const transport_catalogue::TransportCatalogue& transport_catalogue,
                              std::string_view bus);

    graph::Edge<double> MakeBusEdge(domain::Stop* from, domain::Stop* to, const double distance) const;

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
            graph::EdgeId edge_id = graph_->AddEdge({MakeBusEdge(*stop_from, *stop_to, distance)});
            edge_id_to_type_[edge_id] = BusEdgeInfo{bus, span_count, graph_->GetEdges(edge_id).weight};
        }
    }
}

} // namespace transport_router
