#include "transport_router.h"


using namespace std;

namespace transport_router {

void TransportRouter::BuildRouter(const transport_catalogue::TransportCatalogue& transport_catalogue) {
    FillGraph(transport_catalogue);
    router_ = std::make_unique<graph::Router<double>>(*graph_);
    router_->Build();
}

void TransportRouter::FillGraph(const transport_catalogue::TransportCatalogue& transport_catalogue) {
    graph_ = std::make_unique<graph::DirectedWeightedGraph<double>>(transport_catalogue.GetStops().size() * 2);
    FillStopIdDictionaries(transport_catalogue.GetStops());
    AddWaitEdges();
    AddBusEdges(transport_catalogue);
}

void TransportRouter::FillStopIdDictionaries(const std::unordered_map<std::string_view, domain::Stop*>& stops) {
    size_t index = 0;
    for (auto stop : stops) {
        graph::VertexId first_id = index++;
        graph::VertexId second_id = index++;
        stop_as_pair_number_[stop.second] = StopPairVertexId{first_id, second_id};
    }
}

void TransportRouter::AddWaitEdges() {
    for (const auto& [stop, pair_vertex_id] : stop_as_pair_number_) {
        graph::EdgeId edge_id = graph_->AddEdge({pair_vertex_id.bus_wait_begin, pair_vertex_id.bus_wait_end,
                                                static_cast<double>(settings_.bus_wait_time)});
        edge_id_to_type_[edge_id] = WaitEdgeInfo{stop->name, static_cast<double>(settings_.bus_wait_time)};
    }
}

void TransportRouter::AddBusEdges(const transport_catalogue::TransportCatalogue& transport_catalogue) {
    auto& buses = transport_catalogue.GetBuses();
    for (const auto& [bus_name, bus] : buses) {
        if (bus->is_circle) {
            ParseBusRouteOnEdges(bus->route.begin(), bus->route.end(), transport_catalogue, bus_name);
        } else {
            ParseBusRouteOnEdges(bus->route.begin(), bus->route.end(), transport_catalogue, bus_name);
            ParseBusRouteOnEdges(bus->route.rbegin(), bus->route.rend(), transport_catalogue, bus_name);
        }
    }
}

graph::Edge<double> TransportRouter::MakeBusEdge(domain::Stop* from, domain::Stop* to, const double distance) const {
    return {stop_as_pair_number_.at(from).bus_wait_end, stop_as_pair_number_.at(to).bus_wait_begin, (distance / 1000.0 / (settings_.bus_velocity) * 60)};
}

std::optional<RouteInfo> TransportRouter::GetRouteInfo(graph::VertexId from, graph::VertexId to) const {
    std::optional<typename graph::Router<double>::RouteInfo> route_info = router_->BuildRoute(from, to);
    if (route_info) {
        RouteInfo result;
        result.total_time = route_info->weight;
        for (const auto edge : route_info->edges) {
            result.edges.emplace_back(GetEdgeInfo(edge));
        }
        return result;
    }
    return nullopt;
}

const EdgeInfo& TransportRouter::GetEdgeInfo(graph::EdgeId id) const {
    return edge_id_to_type_.at(id);
}

std::optional<StopPairVertexId> TransportRouter::GetPairVertexId(domain::Stop* stop) const {
    if (stop_as_pair_number_.count(stop)) {
        return {stop_as_pair_number_.at(stop)};
    }
    return nullopt;
}

const graph::DirectedWeightedGraph<double>& TransportRouter::GetGraph() const {
    return *graph_;
}

const graph::Router<double>& TransportRouter::GetRouter() const {
    return *router_;
}

const std::unordered_map<domain::Stop*, StopPairVertexId>& TransportRouter::GetStopAsPairNumber() const {
    return stop_as_pair_number_;
}

const std::unordered_map<graph::EdgeId, EdgeInfo> TransportRouter::GetEdgeidToType() const {
    return edge_id_to_type_;
}

const RoutingSettings& TransportRouter::GetRoutingSettings() const {
    return settings_;
}

void TransportRouter::SetSettings(RoutingSettings&& settings) {
    settings_ = move(settings);
}

void TransportRouter::SetGraph(std::vector<graph::Edge<double>>&& edges, std::vector<graph::DirectedWeightedGraph<double>::IncidenceList>&& incidence_lists) {
    graph_ = std::make_unique<graph::DirectedWeightedGraph<double>>(move(edges), move(incidence_lists));
}

void TransportRouter::SetRouter(graph::Router<double>::RoutesInternalData&& routes_internal_data) {
    router_ = std::make_unique<graph::Router<double>>(*graph_, move(routes_internal_data));
}

void TransportRouter::SetStopAsPairNumber(std::unordered_map<domain::Stop*, StopPairVertexId>&& stop_as_pair_number) {
    stop_as_pair_number_ = move(stop_as_pair_number);
}

void TransportRouter::SetEdgeidToType(std::unordered_map<graph::EdgeId, EdgeInfo>&& edge_id_to_type) {
    edge_id_to_type_ = move(edge_id_to_type);
}

} // namespace transport_router
