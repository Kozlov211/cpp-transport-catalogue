#pragma once
#include <unordered_set>
#include <iostream>
#include <string>
#include <deque>

#include "transport_catalogue.h"

namespace InputReader {

namespace Query {

struct Query {
    std::string type;
    std::string name;
    std::string data;
};

} // namespace Quere

class InputReader {
public:
    InputReader(TransportCatalogue::TransportCatalogue& transport_catalogue) : transport_catalogue_(transport_catalogue) {};

    void DataInput(const size_t query_count);

    void AddQuere(const Query::Query&& query);

    std::vector<std::string> GetStops(std::string_view data, bool& is_circle);

    std::vector<std::string> SplitIntoWords(std::string_view text, const char symbol);

    std::pair<std::string, double> GetDistanceToStop (const std::string& data);

private:
    TransportCatalogue::TransportCatalogue& transport_catalogue_;
    std::deque<Query::Query> stops_info;
    std::deque<Query::Query> deque_buses_;

};

std::string ReadLine();

} // namespace InputReader

