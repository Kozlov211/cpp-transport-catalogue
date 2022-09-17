#pragma once
#include <unordered_map>
#include <iostream>
#include <string>
#include <deque>

#include "transport_catalogue.h"

namespace input_reader {

namespace request  {

struct Request {
    std::string type;
    std::string name;
    std::string data;
};

} // namespace request

namespace distance_between_stops {

struct DistanceBetweenStops {
    std::string name;
    std::string stop;
    size_t road_distances;
};

} // namespace distance_between_stops

using namespace request;
using namespace distance_between_stops;

class InputReader {
public:
    InputReader(transport_catalogue::TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue) {};

    void ReadInputData(const size_t query_count);

private:
    transport_catalogue::TransportCatalogue& transport_catalogue_;
    std::deque<Request> deque_requests_;
    std::deque<DistanceBetweenStops> deque_distance_between_stops_;
    std::deque<transport_catalogue::bus::Bus> deque_buses_;
    std::deque<transport_catalogue::stop::Stop> deque_stops_;

private:
    void GetStopsFromData();

    void GetBusesFromData();

    void AppendStopsToTransportCatalogue();

    void AppendBusesToTransportCatalogue();


    std::vector<std::string> GetStops(std::string_view data, bool& is_circle);

    std::vector<std::string> SplitIntoWords(std::string_view text, const char symbol);

    std::pair<std::string, double> GetDistanceToStop (const std::string& data);
};

std::string ReadLine();

} // namespace input_reader
