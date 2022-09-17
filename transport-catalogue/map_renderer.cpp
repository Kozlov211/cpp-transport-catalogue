#include <sstream>

#include "map_renderer.h"

using namespace std;

namespace map_renderer {

using namespace sphere_projector;
using namespace render_settings;

void MapRenderer::AppendCoordinates(const std::vector<coordinates::Coordinates>& geo_coordinates) {
    geo_coordinates_ = geo_coordinates;
}

void MapRenderer::AppendBuses(const std::unordered_map<std::string_view, transport_catalogue::bus::Bus*> buses) {
    buses_ = move(buses);
    BusNameSorting();
}

void MapRenderer::BusNameSorting() {
    for (const auto& [key, value] : buses_) {
        name_of_buses.insert(key);
    }
}

void MapRenderer::RenderMap(ostream& out, const RenderSettings& settings) {
    const SphereProjector proj(geo_coordinates_.begin(), geo_coordinates_.end(), settings.width, settings.height, settings.padding);
    VisualizationRouteLines(proj, settings);
    VisualizationRouteName(proj, settings);
    VisualizationRouteStops(proj, settings);
    VisualizationStopName(proj, settings);
    map_.Render(out);
}

string MapRenderer::GetMapAsString(const RenderSettings& settings) {
    std::ostringstream stream;
    RenderMap(stream, settings);
    return stream.str();
}

void MapRenderer::VisualizationRouteLines(const SphereProjector& proj, const RenderSettings& settings) {
    int color_count = 0;
    for (auto& name : name_of_buses) {
        Svg::Polyline polyline;
        auto& stops = buses_.at(name)->route;
        if (stops.empty()) {
            continue;
        }
        for (auto& stop : stops) {
            polyline.AddPoint(proj(stop->сoordinates))
                    .SetFillColor(Svg::NoneColor)
                    .SetStrokeColor(settings.color_palette[color_count % settings.color_palette.size()])
                    .SetStrokeWidth(settings.line_width)
                    .SetStrokeLineCap(Svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(Svg::StrokeLineJoin::ROUND);
        }
        if (!buses_.at(name)->is_circle) {
            for (auto it = stops.rbegin() + 1; it != stops.rend(); ++it) {
                polyline.AddPoint(proj((*it)->сoordinates))
                        .SetFillColor(Svg::NoneColor)
                        .SetStrokeColor(settings.color_palette[color_count % settings.color_palette.size()])
                        .SetStrokeWidth(settings.line_width)
                        .SetStrokeLineCap(Svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(Svg::StrokeLineJoin::ROUND);
            }
        }
        ++color_count;
        map_.Add(move(polyline));
    }
}

void MapRenderer::VisualizationRouteName(const SphereProjector& proj, const RenderSettings& settings) {
    int color_count = 0;
    for (auto& name : name_of_buses) {
        auto& stops = buses_.at(name)->route;
        if (stops.empty()) {
            continue;
        }
        if (buses_.at(name)->is_circle) {
            Svg::Point point = proj(stops[0]->сoordinates);
            AppendSubstrateForRoutes(name, point, settings);
            AppendTitleForRoutes(name, point, settings.color_palette[color_count % settings.color_palette.size()], settings);
        } else {

            if (stops[0]->name == stops[stops.size() - 1]->name) {
                Svg::Point point_stop_1 = proj(stops[0]->сoordinates);
                AppendSubstrateForRoutes(name, point_stop_1, settings);
                AppendTitleForRoutes(name, point_stop_1, settings.color_palette[color_count % settings.color_palette.size()], settings);
            } else {
                Svg::Point point_stop_1 = proj(stops[0]->сoordinates);
                AppendSubstrateForRoutes(name, point_stop_1, settings);
                AppendTitleForRoutes(name, point_stop_1, settings.color_palette[color_count % settings.color_palette.size()], settings);
                Svg::Point point_stop_2 = proj(stops[stops.size() - 1]->сoordinates);
                AppendSubstrateForRoutes(name, point_stop_2, settings);
                AppendTitleForRoutes(name, point_stop_2, settings.color_palette[color_count % settings.color_palette.size()], settings);
            }
        }
        ++color_count;
    }
}

void MapRenderer::AppendSubstrateForRoutes(std::string_view name, const Svg::Point& point, const RenderSettings& settings) {
    Svg::Text text;
    text.SetPosition(point)
        .SetOffset({settings.bus_label_offset[0], settings.bus_label_offset[1]})
        .SetFontSize(settings.bus_label_font_size)
        .SetFontFamily("Verdana")
        .SetFontWeight("bold")
        .SetData(string(name))
        .SetStrokeWidth(settings.underlayer_width)
        .SetStrokeLineCap(Svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(Svg::StrokeLineJoin::ROUND)
        .SetFillColor(settings.underlayer_color)
        .SetStrokeColor(settings.underlayer_color);
    map_.Add(move(text));
}

void MapRenderer::AppendTitleForRoutes(std::string_view name, const Svg::Point& point,Svg::Color color, const RenderSettings& settings) {
    Svg::Text text;
    text.SetPosition(point)
        .SetFillColor(color)
        .SetOffset({settings.bus_label_offset[0], settings.bus_label_offset[1]})
        .SetFontSize(settings.bus_label_font_size)
        .SetFontFamily("Verdana")
        .SetFontWeight("bold")
        .SetData(string(name));
    map_.Add(move(text));
}

void MapRenderer::VisualizationRouteStops(const SphereProjector& proj, const RenderSettings& settings) {
    Svg::Circle circle;
    GetAllStops();
    for (auto& stop : all_stops_) {
        circle.SetCenter(proj(stop->сoordinates))
                .SetRadius(settings.stop_radius)
                .SetFillColor("white");
        map_.Add(move(circle));
    }
}

void MapRenderer::GetAllStops() {
    for (auto& name : name_of_buses) {
        auto& stops = buses_.at(name)->route;
        for (auto& stop : stops) {
            all_stops_.insert(stop);
        }
    }
}

void MapRenderer::VisualizationStopName(const SphereProjector& proj, const RenderSettings& settings) {
    for (auto& stop : all_stops_) {
        AppendSubstrateForNameStop(stop->name, proj(stop->сoordinates), settings);
        AppendTitleForNameStop(stop->name, proj(stop->сoordinates), "black", settings);
    }
}

void MapRenderer::AppendSubstrateForNameStop(std::string_view name, const Svg::Point& point, const RenderSettings& settings) {
    Svg::Text text;
    text.SetPosition(point)
        .SetOffset({settings.stop_label_offset[0], settings.stop_label_offset[1]})
        .SetFontSize(settings.stop_label_font_size)
        .SetFontFamily("Verdana")
        .SetData(string(name))
        .SetStrokeWidth(settings.underlayer_width)
        .SetStrokeLineCap(Svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(Svg::StrokeLineJoin::ROUND)
        .SetFillColor(settings.underlayer_color)
        .SetStrokeColor(settings.underlayer_color);
    map_.Add(move(text));
}

void MapRenderer::AppendTitleForNameStop(std::string_view name, const Svg::Point& point, Svg::Color color, const RenderSettings& settings) {
    Svg::Text text;
    text.SetPosition(point)
        .SetFillColor(color)
        .SetOffset({settings.stop_label_offset[0], settings.stop_label_offset[1]})
        .SetFontSize(settings.stop_label_font_size)
        .SetFontFamily("Verdana")
        .SetData(string(name));
    map_.Add(move(text));
}


} // namespace map_renderer
