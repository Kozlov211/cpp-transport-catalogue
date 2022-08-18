#pragma once
#include <unordered_map>
#include <iostream>
#include <string>
#include <deque>

#include "transport_catalogue.h"

namespace InputReader {

namespace Request  {

struct Request {
    std::string type;
    std::string name;
    std::string data;
};

} // namespace Request

namespace DistanceBetweenStops {

struct DistanceBetweenStops {
    std::string name;
    std::string stop;
    uint32_t road_distances;
};

}

class InputReader {
public:
    InputReader(TransportCatalogue::TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue) {};

    void ReadInputData(const size_t query_count);

private:
    TransportCatalogue::TransportCatalogue& transport_catalogue_;
    std::deque<Request::Request> deque_requests_;
    std::deque<DistanceBetweenStops::DistanceBetweenStops> deque_distance_between_stops_;
    std::deque<TransportCatalogue::Bus::Bus> deque_buses_;
    std::deque<TransportCatalogue::Stop::Stop> deque_stops_;

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

} // namespace InputReader
