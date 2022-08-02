#pragma once
#include <unordered_set>
#include <iostream>
#include <string>

#include "transport_catalogue.h"

namespace InputReader {

namespace Quere {

struct Quere {
    std::string type;
    std::string name;
    std::string data;
};

} // namespace Quere

class InputReader {
public:
    InputReader(TransportCatalogue::TransportCatalogue& transport_catalogue) : transport_catalogue(transport_catalogue) {};

    void AddQuere(const Quere::Quere&& quere);

    std::vector<std::string> GetStops(std::string_view data, bool& is_circle);

    std::vector<std::string> SplitIntoWords(std::string_view text, const char symbol);

    std::pair<std::string, double> GetDistanceToStop (const std::string& data);

private:
    TransportCatalogue::TransportCatalogue& transport_catalogue;

};

std::string ReadLine();

} // namespace InputReader

std::istream& operator>>(std::istream& is, InputReader::InputReader& reader);

