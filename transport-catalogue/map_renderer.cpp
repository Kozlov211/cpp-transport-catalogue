#include "map_renderer.h"

using namespace std;

namespace MapRenderer {

void MapRenderer::AppendRenderSettings(const JsonReader::JsonReader& json_reader) {
    settings_ = json_reader.GetRenderSettings();
}

void MapRenderer::AppendCoordinates(const std::vector<Coordinates>& geo_coordinates) {
    geo_coordinates_ = geo_coordinates;
}

void MapRenderer::AppendBuses(const std::unordered_map<std::string_view, TransportCatalogue::Bus::Bus*>& buses) {
    buses_ = buses;
    BusNameSorting();
}

void MapRenderer::BusNameSorting() {
    for (const auto& [key, value] : buses_) {
        name_of_buses.insert(key);
    }
}

void MapRenderer::RenderMap(ostream& out) {
    const SphereProjector::SphereProjector proj(geo_coordinates_.begin(), geo_coordinates_.end(), settings_.width, settings_.height, settings_.padding);
    AppendPolylinesRoutesForMap(proj);
    AppendRouteNamesForMap(proj);
    GetAllStops();
    AppendStopSymbolsForMap(proj);
    AppendStopNameForMap(proj);
    map_.Render(out);
}

string MapRenderer::GetMapAsString() {
    std::ostringstream stream;
    RenderMap(stream);
    return stream.str();
}

void MapRenderer::AppendPolylinesRoutesForMap(const SphereProjector::SphereProjector& proj) {
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
                    .SetStrokeColor(settings_.color_palette[color_count % settings_.color_palette.size()])
                    .SetStrokeWidth(settings_.line_width)
                    .SetStrokeLineCap(Svg::StrokeLineCap::ROUND)
                    .SetStrokeLineJoin(Svg::StrokeLineJoin::ROUND);
        }
        if (!buses_.at(name)->is_circle) {
            for (auto it = stops.rbegin() + 1; it != stops.rend(); ++it) {
                polyline.AddPoint(proj((*it)->сoordinates))
                        .SetFillColor(Svg::NoneColor)
                        .SetStrokeColor(settings_.color_palette[color_count % settings_.color_palette.size()])
                        .SetStrokeWidth(settings_.line_width)
                        .SetStrokeLineCap(Svg::StrokeLineCap::ROUND)
                        .SetStrokeLineJoin(Svg::StrokeLineJoin::ROUND);
            }
        }
        ++color_count;
        map_.Add(move(polyline));
    }
}

void MapRenderer::AppendRouteNamesForMap(const SphereProjector::SphereProjector& proj) {
    int color_count = 0;
    for (auto& name : name_of_buses) {
        auto& stops = buses_.at(name)->route;
        if (stops.empty()) {
            continue;
        }
        if (buses_.at(name)->is_circle) {
            Svg::Point point = proj(stops[0]->сoordinates);
            AppendSubstrateForRoutes(name, point);
            AppendTitleForRoutes(name, point, settings_.color_palette[color_count % settings_.color_palette.size()]);
        } else {

            if (stops[0]->name == stops[stops.size() - 1]->name) {
                Svg::Point point_stop_1 = proj(stops[0]->сoordinates);
                AppendSubstrateForRoutes(name, point_stop_1);
                AppendTitleForRoutes(name, point_stop_1, settings_.color_palette[color_count % settings_.color_palette.size()]);
            } else {
                Svg::Point point_stop_1 = proj(stops[0]->сoordinates);
                AppendSubstrateForRoutes(name, point_stop_1);
                AppendTitleForRoutes(name, point_stop_1, settings_.color_palette[color_count % settings_.color_palette.size()]);
                Svg::Point point_stop_2 = proj(stops[stops.size() - 1]->сoordinates);
                AppendSubstrateForRoutes(name, point_stop_2);
                AppendTitleForRoutes(name, point_stop_2, settings_.color_palette[color_count % settings_.color_palette.size()]);
            }
        }
        ++color_count;
    }
}

void MapRenderer::AppendSubstrateForRoutes(std::string_view name, const Svg::Point& point) {
    Svg::Text text;
    text.SetPosition(point)
        .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
        .SetFontSize(settings_.bus_label_font_size)
        .SetFontFamily("Verdana")
        .SetFontWeight("bold")
        .SetData(string(name))
        .SetStrokeWidth(settings_.underlayer_width)
        .SetStrokeLineCap(Svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(Svg::StrokeLineJoin::ROUND)
        .SetFillColor(settings_.underlayer_color)
        .SetStrokeColor(settings_.underlayer_color);
    map_.Add(move(text));
}

void MapRenderer::AppendTitleForRoutes(std::string_view name, const Svg::Point& point,Svg::Color color) {
    Svg::Text text;
    text.SetPosition(point)
        .SetFillColor(color)
        .SetOffset({settings_.bus_label_offset[0], settings_.bus_label_offset[1]})
        .SetFontSize(settings_.bus_label_font_size)
        .SetFontFamily("Verdana")
        .SetFontWeight("bold")
        .SetData(string(name));
    map_.Add(move(text));
}

void MapRenderer::AppendStopSymbolsForMap(const SphereProjector::SphereProjector& proj) {
    Svg::Circle circle;
    for (auto& stop : all_stops_) {
        circle.SetCenter(proj(stop->сoordinates))
                .SetRadius(settings_.stop_radius)
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

void MapRenderer::AppendStopNameForMap(const SphereProjector::SphereProjector& proj) {
    for (auto& stop : all_stops_) {
        AppendSubstrateForNameStop(stop->name, proj(stop->сoordinates));
        AppendTitleForNameStop(stop->name, proj(stop->сoordinates), "black");
    }
}

void MapRenderer::AppendSubstrateForNameStop(std::string_view name, const Svg::Point& point) {
    Svg::Text text;
    text.SetPosition(point)
        .SetOffset({settings_.stop_label_offset[0], settings_.stop_label_offset[1]})
        .SetFontSize(settings_.stop_label_font_size)
        .SetFontFamily("Verdana")
        .SetData(string(name))
        .SetStrokeWidth(settings_.underlayer_width)
        .SetStrokeLineCap(Svg::StrokeLineCap::ROUND)
        .SetStrokeLineJoin(Svg::StrokeLineJoin::ROUND)
        .SetFillColor(settings_.underlayer_color)
        .SetStrokeColor(settings_.underlayer_color);
    map_.Add(move(text));
}

void MapRenderer::AppendTitleForNameStop(std::string_view name, const Svg::Point& point, Svg::Color color) {
    Svg::Text text;
    text.SetPosition(point)
        .SetFillColor(color)
        .SetOffset({settings_.stop_label_offset[0], settings_.stop_label_offset[1]})
        .SetFontSize(settings_.stop_label_font_size)
        .SetFontFamily("Verdana")
        .SetData(string(name));
    map_.Add(move(text));
}


} // namespace MapRenderer
