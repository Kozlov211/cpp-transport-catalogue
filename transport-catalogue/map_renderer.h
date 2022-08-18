#pragma once

#include <algorithm>

#include "geo.h"
#include "json_reader.h"

namespace SphereProjector {

class SphereProjector {
public:
    template <typename PointInputIt>
    SphereProjector(PointInputIt points_begin, PointInputIt points_end, double max_width, double max_height, double padding);

    Svg::Point operator()(Coordinates coords) const {
        return {
            (coords.lng - min_lon_) * zoom_coeff_ + padding_,
            (max_lat_ - coords.lat) * zoom_coeff_ + padding_
        };
    }

private:
    double padding_;
    double min_lon_ = 0;
    double max_lat_ = 0;
    double zoom_coeff_ = 0;

private:
    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }
};

template <typename PointInputIt>
SphereProjector::SphereProjector(PointInputIt points_begin, PointInputIt points_end,
                double max_width, double max_height, double padding) : padding_(padding) {
    if (points_begin == points_end) {
        return;
    }
    const auto [left_it, right_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lng < rhs.lng; });
    min_lon_ = left_it->lng;
    const double max_lon = right_it->lng;
    const auto [bottom_it, top_it] = std::minmax_element(
        points_begin, points_end,
        [](auto lhs, auto rhs) { return lhs.lat < rhs.lat; });
    const double min_lat = bottom_it->lat;
    max_lat_ = top_it->lat;
    std::optional<double> width_zoom;
    if (!IsZero(max_lon - min_lon_)) {
        width_zoom = (max_width - 2 * padding) / (max_lon - min_lon_);
    }
    std::optional<double> height_zoom;
    if (!IsZero(max_lat_ - min_lat)) {
        height_zoom = (max_height - 2 * padding) / (max_lat_ - min_lat);
    }
    if (width_zoom && height_zoom) {
        zoom_coeff_ = std::min(*width_zoom, *height_zoom);
    } else if (width_zoom) {
        zoom_coeff_ = *width_zoom;
    } else if (height_zoom) {
        zoom_coeff_ = *height_zoom;
    }
}

} // namespace SphereProjector

namespace MapRenderer {

class MapRenderer {
public:
    void AppendRenderSettings(const JsonReader::JsonReader& json_reader);

    void AppendBuses(const std::unordered_map<std::string_view, TransportCatalogue::Bus::Bus*>& buses);

    void AppendCoordinates(const std::vector<Coordinates>& geo_coordinates);

    void RenderMap(std::ostream& out);

    std::string GetMapAsString();

private:
    JsonReader::RenderSettings::RenderSettings settings_;
    std::vector<Coordinates> geo_coordinates_;
    std::unordered_map<std::string_view, TransportCatalogue::Bus::Bus*> buses_;
    std::set<std::string_view> name_of_buses;
    std::set<TransportCatalogue::Stop::Stop*, TransportCatalogue::Stop::CompStop> all_stops_;
    Svg::Document map_;

private:
    void ReadGeoCoordinatesFromStops();

    void BusNameSorting();

    void AppendPolylinesRoutesForMap(const SphereProjector::SphereProjector& proj);

    void AppendRouteNamesForMap(const SphereProjector::SphereProjector& proj);

    void AppendSubstrateForRoutes(std::string_view name, const Svg::Point& point);

    void AppendTitleForRoutes(std::string_view name, const Svg::Point& point, Svg::Color color);

    void AppendStopSymbolsForMap(const SphereProjector::SphereProjector& proj);

    void GetAllStops();

    void AppendStopNameForMap(const SphereProjector::SphereProjector& proj);

    void AppendSubstrateForNameStop(std::string_view name, const Svg::Point& point);

    void AppendTitleForNameStop(std::string_view name, const Svg::Point& point, Svg::Color color);
};

} // namespace MapRenderer

