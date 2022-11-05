#pragma once

#include "domain.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "transport_router.h"

#include <deque>
#include <optional>
#include <iostream>
#include <transport_router.pb.h>
#include <transport_catalogue.pb.h>
#include <svg.pb.h>
#include <unordered_map>


namespace serial_handler {

class SerialHandler {
public:
    SerialHandler(domain::SerializationSettings&& settings) : settings_(std::move(settings)) {}

    void Serialization(std::ostream& output);

    void SerializationTransportCatalogue(const transport_catalogue::TransportCatalogue& transport_catalogue);

    void SerializationMapRenderSettings(const map_renderer::MapRenderer& map_renderer);

    void SerializationTransoprtRouter(const transport_router::TransportRouter& transport_router);

    void Deserialize(std::istream& input);

    transport_catalogue::TransportCatalogue GetTransportCatalogue();

    domain::RenderSettings GetMapRenderSettings();

    transport_router::TransportRouter GetTransportRouter(const transport_catalogue::TransportCatalogue& transport_catalogue);

    const domain::SerializationSettings& GetSettings() const;

private:
    domain::SerializationSettings settings_;
    transport_catalogue_serialize::TransportCatalogue transport_catalogue_proto_;

private:
    void SerializationStops(const std::deque<domain::Stop>& stops);

    void SerializationBuses(const std::deque<domain::Bus>& buses);

    void SerializationDistanceBetweenStops(const std::unordered_map<std::pair<domain::Stop*, domain::Stop*>, size_t, transport_catalogue::detail::Hash>& distance_between_stops_);

    void DeserializeStops(transport_catalogue::TransportCatalogue& transport_catalogue);

    void DeserializeBuses(transport_catalogue::TransportCatalogue& transport_catalogue);

    void DeserializeDistanceBetweenStops(transport_catalogue::TransportCatalogue& transport_catalogue);

    void SerializationMapRenderSettings(const domain::RenderSettings& render_settings);

    domain::RenderSettings DeserializeMapRenderSettings();

    void SerializationColor(svg_proto::Color* color_proto, const svg::Color& color);

    svg::Color DeserializeColor(const svg_proto::Color& color_proto);

    void SerializationRoutingSettings(const domain::RoutingSettings& routing_settings);

    void SerializationGraph(const graph::DirectedWeightedGraph<double>& graph);

    void SerializationEdgesFromGraph(const std::vector<graph::Edge<double>>& edges);

    void SerializationIncidenceListsFromGraph(const std::vector<graph::DirectedWeightedGraph<double>::IncidenceList>& incidence_lists);

    void SerializationRouter(const graph::Router<double>& router);

    void SerializationStopAsPairNumber(const std::unordered_map<domain::Stop*, domain::StopPairVertexId>& stop_as_pair_number_);

    void SerializationEdgeidToType(const std::unordered_map<graph::EdgeId, domain::EdgeInfo>& edge_id_to_type_);

    domain::RoutingSettings DeserializeRoutingSettings();

    std::vector<graph::Edge<double>> DeserializeEdgesFromGraph();

    std::vector<graph::DirectedWeightedGraph<double>::IncidenceList> DeserializeIncidenceListsFromGraph();

    graph::Router<double>::RoutesInternalData DeserializeRouter();

    std::unordered_map<domain::Stop*, domain::StopPairVertexId> DeserializeStopAsPairNumber(const transport_catalogue::TransportCatalogue& transport_catalogue);

    std::unordered_map<graph::EdgeId, domain::EdgeInfo> DeserializeEdgeidToType(const transport_catalogue::TransportCatalogue& transport_catalogue);

};

}


