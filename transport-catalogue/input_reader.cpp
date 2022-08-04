#include "input_reader.h"

using namespace std;

namespace InputReader {

void InputReader::DataInput(const size_t query_count) {
    for(size_t i = 0; i < query_count; ++i) {
        string line;
        getline(cin, line);
        size_t space = line.find_first_of(' ');
        size_t point = line.find_first_of(':');
        Query::Query query;
        query.type = line.substr(0, space);
        query.name = line.substr(space + 1, point - space - 1);
        query.data = line.substr(point + 2, line.size() - point);
        if (query.type == "Bus") {
            deque_buses_.push_back(move(query));
        } else {
            vector<string> data = SplitIntoWords(query.data, ',');
            transport_catalogue_.AddStop(query.name, {stod(data[0]), stod(data[1])});
            stops_info.push_back(move(query));
        }
    }
    while(!stops_info.empty()) {
        Query::Query query = stops_info.front();
        pair<string, uint32_t> distance_to_stop;
        vector<string> data = SplitIntoWords(query.data, ',');
        for (size_t i = 2; i < data.size(); ++i) {
            distance_to_stop = GetDistanceToStop(data[i]);
            transport_catalogue_.SetDistanceToStop(query.name, distance_to_stop.first, distance_to_stop.second);
        }
        stops_info.pop_front();
    }
    while(!deque_buses_.empty()) {
        Query::Query query = deque_buses_.front();
        bool is_circle;
        vector<string> route = GetStops(query.data, is_circle);
        vector<string_view> rout_b;
        transport_catalogue_.AddBusRoute(query.name, is_circle, rout_b);
        for (string& stop : route) {
           auto key_stop = transport_catalogue_.GetKeyStop(stop);
           auto key_bus = transport_catalogue_.GetKeyBus(query.name);
           transport_catalogue_.AddBusToStop(key_stop->first, key_bus->first);
           rout_b.push_back(key_stop->first);
        }
        transport_catalogue_.AddBusRoute(query.name, is_circle, rout_b);
        deque_buses_.pop_front();
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

} // InputReader

