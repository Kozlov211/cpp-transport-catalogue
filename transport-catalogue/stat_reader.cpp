#include <sstream>
#include <iomanip>

#include "stat_reader.h"

using namespace std;

namespace StatReader {


} // namespace StatReader

std::istream& operator>>(std::istream& is, StatReader::StatReader& reader) {
    string line;
    getline(is, line);
    size_t space = line.find_first_of(' ');
    string type = line.substr(0, space);
    string name = line.substr(space + 1, line.size() - space);
    reader.request.type = type;
    reader.request.name = name;
    return is;
}

std::ostream& operator<<(std::ostream& os, StatReader::StatReader& reader) {
    if (reader.request.type == "Bus") {
        if (reader.transport_catalogue.CheckBus(reader.request.name)) {
            transport_catalogue::bus::Bus* bus = reader.transport_catalogue.GetBus(reader.request.name);
            os << "Bus " << bus->name << ": " << reader.transport_catalogue.GetNumberStopsOnTheRoute(bus) << " stops on route, "
               << reader.transport_catalogue.GetNumberUniqueStopsOnTheRoute(bus) << " unique stops, "
               << setprecision(6) << reader.transport_catalogue.GetRoadLength(bus) << " route length, "
               << reader.transport_catalogue.GetCurvatureRoute(bus) << " curvature\n";
        } else {
            os << "Bus " << reader.request.name << ": not found\n";
        }
    } else {
        if (reader.transport_catalogue.CheckStop(reader.request.name)) {
            set<string_view> buses = reader.transport_catalogue.GetBusesPassingTheStop(reader.request.name);
            if (!buses.empty()) {
                os << "Stop " << reader.request.name << ": buses";
                for (string_view element : buses) {
                    os << " " << element;
                }
                os << "\n";
            } else {
                os << "Stop " << reader.request.name << ": no buses\n";
            }

        } else {
            os << "Stop " << reader.request.name << ": not found\n";
        }
    }
    return os;
}
