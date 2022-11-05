#include <sstream>

#include "request_handler.h"
#include "json_builder.h"


namespace request_handler {
using namespace std;
using namespace json_reader;
using namespace json;
using namespace domain;
using namespace transport_catalogue;
using namespace transport_router;
using namespace map_renderer;

string RequestHandler::GetResponseToRequest(const TransportCatalogue& transport_catalogue, const MapRenderer& map_renderer,
                                            const TransportRouter& transport_router) {
    Array response;
    Builder builder;
    builder.StartArray();
    std::ostringstream out;
    for (auto& request: requests_) {
        if (holds_alternative<RequestMap>(request)) {
            BuildJsonMap(builder, transport_catalogue, std::get<RequestMap>(request), map_renderer);
        } else if (holds_alternative<RequestRoute>(request)) {
            BuildJsonRoute(builder, transport_catalogue, std::get<RequestRoute>(request), transport_router);
        } else {
            RequestBusOrStop request_bus_or_stop = std::get<RequestBusOrStop>(request);
            if (request_bus_or_stop.type == "Bus" && transport_catalogue.CheckBus(request_bus_or_stop.name)) {
                BuildJsonBus(builder, transport_catalogue, request_bus_or_stop);
            } else if (request_bus_or_stop.type == "Stop" && transport_catalogue.CheckStop(request_bus_or_stop.name)) {
                BuildJsonStop(builder, transport_catalogue, request_bus_or_stop);
            } else {
                BuildJsonErrorMessage(builder, request_bus_or_stop.id);
            }
        }
    }
    Print(Document{builder.EndArray().Build()}, out);
    return out.str();
}

void RequestHandler::BuildJsonStop(Builder& builder, const TransportCatalogue& transport_catalogue, const RequestBusOrStop& request) const {
    domain::Stop* stop = transport_catalogue.GetStop(request.name);
    builder.StartDict().Key("request_id"s).Value(request.id);
    builder.Key("buses"s).StartArray();
    for (const auto& bus : transport_catalogue.GetBusesPassingTheStop(stop->name)) {
        builder.Value(string(bus));
    }
    builder.EndArray().EndDict();
}

void RequestHandler::BuildJsonBus(Builder& builder, const TransportCatalogue& transport_catalogue, const RequestBusOrStop& request) const {
    domain::Bus* bus = transport_catalogue.GetBus(request.name);
    builder.StartDict().Key("request_id"s).Value(request.id).Key("curvature"s).Value(transport_catalogue.GetCurvatureRoute(bus))
            .Key("route_length"s).Value(transport_catalogue.GetRoadLength(bus)).Key("stop_count"s).Value(static_cast<int>(transport_catalogue.GetNumberStopsOnTheRoute(bus)))
            .Key("unique_stop_count"s).Value(static_cast<int>(transport_catalogue.GetNumberUniqueStopsOnTheRoute(bus))).EndDict();
}

void RequestHandler::BuildJsonMap(Builder& builder, const TransportCatalogue& transport_catalogue, const RequestMap& request, const MapRenderer& map_render) const {
    builder.StartDict().Key("request_id"s).Value(request.id).Key("map"s)
            .Value(map_render.GetMapAsString()).EndDict();
}

void RequestHandler::BuildJsonRoute(Builder& builder, const TransportCatalogue& transport_catalogue, const RequestRoute& request, const TransportRouter& transport_router) const {
    if (transport_catalogue.CheckStop(request.from) && transport_catalogue.CheckStop(request.to)) {
        std::optional<StopPairVertexId> pair_vertex_id_stop_from = transport_router.GetPairVertexId(transport_catalogue.GetStop(request.from));
        std::optional<StopPairVertexId> pair_vertex_id_stop_to = transport_router.GetPairVertexId(transport_catalogue.GetStop(request.to));
        std::optional<RouteInfo> route_info = transport_router.GetRouteInfo(pair_vertex_id_stop_from->bus_wait_begin, pair_vertex_id_stop_to->bus_wait_begin);
        if (route_info) {
            builder.StartDict().Key("request_id"s).Value(request.id).Key("total_time"s).Value(route_info->total_time).Key("items"s).StartArray();
            for (auto& info : route_info->edges) {
                if (info.index() == 1) {
                    BuildJsonBusEdge(builder, get<BusEdgeInfo>(info));
                } else {
                    BuildJsonWaitEdge(builder, get<WaitEdgeInfo>(info));
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

void RequestHandler::BuildJsonBusEdge(Builder& builder, const BusEdgeInfo& bus_edge_info) const {
    builder.StartDict().Key("type"s).Value("Bus"s)
            .Key("bus"s).Value(string(bus_edge_info.bus_name))
            .Key("span_count"s).Value(static_cast<int>(bus_edge_info.span_count))
            .Key("time"s).Value(bus_edge_info.time).EndDict();
}

void RequestHandler::BuildJsonWaitEdge(Builder& builder, const WaitEdgeInfo& wait_edge_info) const {
    builder.StartDict().Key("type"s).Value("Wait"s)
            .Key("stop_name"s).Value(string(wait_edge_info.stop_name))
            .Key("time"s).Value(wait_edge_info.time).EndDict();
}

void RequestHandler::BuildJsonErrorMessage(Builder& builder, const int request_id) const {
    builder.StartDict().Key("error_message"s).Value("not found"s).Key("request_id"s).Value(request_id).EndDict();
}

} // namespace RequestHandler
