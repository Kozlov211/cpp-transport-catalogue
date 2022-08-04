#pragma once
#include <string>
#include <iostream>

#include "transport_catalogue.h"

namespace StatReader {

namespace Quere {

struct Quere {
    bool is_found = false;
    std::string type;
    std::string name;
};

} // namespace Quere

class StatReader {
public:
    StatReader(TransportCatalogue::TransportCatalogue& transport_catalogue) : transport_catalogue(transport_catalogue) {}

    void DefineQuery (const std::string& type, const std::string& name);

    TransportCatalogue::TransportCatalogue& transport_catalogue;

    Quere::Quere quere_;

};

} // napespace StatReader

std::istream& operator>>(std::istream& is, StatReader::StatReader& reader);

std::ostream& operator<<(std::ostream& is, StatReader::StatReader& reader);

