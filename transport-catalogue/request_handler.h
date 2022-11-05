#pragma once

#include "json_reader.h"
#include "json_builder.h"
#include "map_renderer.h"
#include "domain.h"
#include "transport_catalogue.h"
#include "transport_router.h"


namespace request_handler {

class RequestHandler {
public:
    RequestHandler(std::deque<domain::RequestData>&& requests) : requests_(move(requests)) {}

    std::string GetResponseToRequest(const transport_catalogue::TransportCatalogue& transport_catalogue,
                                     const map_renderer::MapRenderer& map_renderer,
                                     const transport_router::TransportRouter& transport_router);

private:
    std::deque<domain::RequestData> requests_;

private:
    void BuildJsonStop(json::Builder& builder, const transport_catalogue::TransportCatalogue& transport_catalogue, const domain::RequestBusOrStop& request) const;

    void BuildJsonBus(json::Builder& builder, const transport_catalogue::TransportCatalogue& transport_catalogue, const domain::RequestBusOrStop& request) const;

    void BuildJsonMap(json::Builder& builder, const transport_catalogue::TransportCatalogue& transport_catalogue, const domain::RequestMap& request, const map_renderer::MapRenderer& map_renderer) const;

    void BuildJsonRoute(json::Builder& builder, const transport_catalogue::TransportCatalogue& transport_catalogue, const domain::RequestRoute& request, const transport_router::TransportRouter& transport_router) const;

    void BuildJsonBusEdge(json::Builder& builder, const domain::BusEdgeInfo& bus_edge_info) const;

    void BuildJsonWaitEdge(json::Builder& builder, const domain::WaitEdgeInfo& wait_edge_info) const;

    void BuildJsonErrorMessage(json::Builder& builder, const int request_id) const;

};

} // namespace RequestHandler
