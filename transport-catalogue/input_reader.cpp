#include "input_reader.h"

using namespace std;

namespace input_reader {

using namespace request;
using namespace distance_between_stops;

void InputReader::ReadInputData(const size_t query_count) {
    for(size_t i = 0; i < query_count; ++i) {
        string line;
        getline(cin, line);
        size_t space = line.find_first_of(' ');
        size_t point = line.find_first_of(':');
        Request request;
        request.type = line.substr(0, space);
        request.name = line.substr(space + 1, point - space - 1);
        request.data = line.substr(point + 2, line.size() - point);
        deque_requests_.push_back(move(request));
    }
    GetStopsFromData();
    AppendStopsToTransportCatalogue();
    GetBusesFromData();
    AppendBusesToTransportCatalogue();
}

void InputReader::AppendStopsToTransportCatalogue() {
    for (auto& stop : deque_stops_) {
        transport_catalogue_.AppendStop(stop.name, &stop);
    }
    for (const auto& stop : deque_distance_between_stops_) {
        transport_catalogue_.AppendDistanceToStop(transport_catalogue_.GetStop(stop.name), transport_catalogue_.GetStop(stop.stop), stop.road_distances);
    }
}

void InputReader::GetStopsFromData() {
    for (auto& request : deque_requests_) {
        if (request.type == "Stop") {
            vector<string> data = SplitIntoWords(request.data, ',');
            transport_catalogue::stop::Stop stop {request.name, {stod(data[0]), stod(data[1])}};
            pair<string, uint32_t> distance_to_stop;
            for (size_t i = 2; i < data.size(); ++i) {
                distance_to_stop = GetDistanceToStop(data[i]);
                DistanceBetweenStops distance_between_stop;
                distance_between_stop.name = request.name;
                distance_between_stop.stop = distance_to_stop.first;
                distance_between_stop.road_distances = distance_to_stop.second;
                deque_distance_between_stops_.push_back(move(distance_between_stop));
            }
            deque_stops_.push_back(move(stop));
        }
    }
}

void InputReader::AppendBusesToTransportCatalogue() {
    for (auto& bus : deque_buses_) {
        transport_catalogue_.AppendBus(bus.name, &bus);
        for (auto* stop : bus.route) {
            transport_catalogue_.AppendBusToStop(stop, &bus);
        }
    }
}

void InputReader::GetBusesFromData() {
    for (auto& request : deque_requests_) {
        if (request.type == "Bus") {
            transport_catalogue::bus::Bus bus;
            bus.name = request.name;
            bool is_circle;
            vector<string> route = GetStops(request.data, is_circle);
            bus.is_circle = is_circle;
            for (const auto& stop : route) {
                bus.route.push_back(transport_catalogue_.GetStop(stop));
            }
            deque_buses_.push_back(move(bus));
        }
    }
}


vector<string> InputReader::GetStops(string_view data, bool& is_circle) {
    if (data.find(">") != string::npos) {
        is_circle = true;
        return SplitIntoWords(data, '>');
    } else {
        is_circle = false;
        return SplitIntoWords(data, '-');
    }
}

vector<string> InputReader::SplitIntoWords(string_view text, const char symbol) {
    vector<string> words;
    string word;
    size_t delete_space;
    if (symbol == ',') {
        delete_space = 0;
    } else {
        delete_space = 1;
    }
    while (text.find_first_of(symbol) != string::npos) {
        size_t point = text.find_first_of(symbol);
        word = text.substr(0, point - delete_space);
        words.push_back(move(word));
        text = text.substr(point + 2, text.size() - point - 1);
    }
    word = text;
    words.push_back(move(word));
    return words;
}

pair<string, double> InputReader::GetDistanceToStop (const string& data) {
    size_t space = data.find_first_of(' ');
    return {data.substr(space + 4), stoi(data.substr(0, space - 1))};
}


string ReadLine() {
    string s;
    getline(cin, s);
    return s;
}

} // input_reader
