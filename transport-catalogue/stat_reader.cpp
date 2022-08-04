#include <sstream>
#include <iomanip>

#include "stat_reader.h"

using namespace std;

namespace StatReader {

void StatReader::DefineQuery (const std::string& type, const std::string& name) {
    if (type == "Bus") {
        quere_.is_found = transport_catalogue.CheckBus(name);
        quere_.type = type;
        quere_.name = name;
    }
    if (type == "Stop") {
        quere_.is_found = transport_catalogue.CheckStop(name);
        quere_.type = type;
        quere_.name = name;

    }
}

} // namespace StatReader

std::istream& operator>>(std::istream& is, StatReader::StatReader& reader) {
    string line;
    getline(is, line);
    size_t space = line.find_first_of(' ');
    string type = line.substr(0, space);
    string name = line.substr(space + 1, line.size() - space);
    reader.DefineQuery(type, name);
    return is;
}

std::ostream& operator<<(std::ostream& os, StatReader::StatReader& reader) {
    if (reader.quere_.type == "Bus") {
        if (reader.quere_.is_found) {
            os << "Bus " << reader.quere_.name << ": " << reader.transport_catalogue.GetBusStops(reader.quere_.name) << " stops on route, "
               << reader.transport_catalogue.GetBusUniqStops(reader.quere_.name) << " unique stops, "
               << setprecision(6) << reader.transport_catalogue.GetRoadDistance(reader.quere_.name) << " route length, "
               << reader.transport_catalogue.GetCurvatureRoute(reader.quere_.name) << " curvature\n";
        } else {
            os << "Bus " << reader.quere_.name << ": not found\n";
        }
    } else {
        if (reader.quere_.is_found) {
            set<string_view> buses = reader.transport_catalogue.GetBusesPassingTheStop(reader.quere_.name);
            if (!buses.empty()) {
                os << "Stop " << reader.quere_.name << ": buses";
                for (string_view element : buses) {
                    os << " " << element;
                }
                os << "\n";
            } else {
                os << "Stop " << reader.quere_.name << ": no buses\n";
            }

        } else {
            os << "Stop " << reader.quere_.name << ": not found\n";
        }
    }
    return os;
}


