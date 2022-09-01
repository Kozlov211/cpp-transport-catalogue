#pragma once
#include <string>
#include <iostream>

#include "transport_catalogue.h"

namespace StatReader {

namespace Request {

struct Request  {
    std::string type;
    std::string name;
};

} // namespace Quere

class StatReader {
public:
    StatReader(transport_catalogue::TransportCatalogue& transport_catalogue) : transport_catalogue(transport_catalogue) {}

    transport_catalogue::TransportCatalogue& transport_catalogue;

    Request::Request request;

};

} // napespace StatReader

std::istream& operator>>(std::istream& is, StatReader::StatReader& reader);

std::ostream& operator<<(std::ostream& is, StatReader::StatReader& reader);
