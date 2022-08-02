#include "input_reader.h"

using namespace std;

namespace InputReader {

void InputReader::AddQuere(const Quere::Quere&& quere) {
    if (quere.type == "Stop") {
        vector<string> data = SplitIntoWords(quere.data, ',');
        std::pair<std::string, uint32_t> distance_to_stop;
        transport_catalogue.AddStop(move(quere.name), {stod(data[0]), stod(data[1])}, false);
        for (size_t i = 2; i < data.size(); ++i) {
            distance_to_stop = GetDistanceToStop(data[i]);
            if (transport_catalogue.CheckStop(distance_to_stop.first)) {
                auto key_stop = transport_catalogue.GetKeyStop(distance_to_stop.first);
                transport_catalogue.AddDistanceToStop(quere.name, key_stop->first, distance_to_stop.second);
            } else {
                transport_catalogue.AddStop(move(distance_to_stop.first), {}, true);
                auto key_stop = transport_catalogue.GetKeyStop(distance_to_stop.first);
                transport_catalogue.AddDistanceToStop(quere.name, key_stop->first, distance_to_stop.second);
            }
        }

    } else {
        bool is_circle;
        vector<string> route = GetStops(quere.data, is_circle);
        vector<string_view> rout_b;
        transport_catalogue.AddBusRoute(quere.name, is_circle, rout_b);
        for (string& stop : route) {
            transport_catalogue.AddStop(move(stop), {}, true);
            auto key_stop = transport_catalogue.GetKeyStop(stop);
            auto key_bus = transport_catalogue.GetKeyBus(quere.name);
            transport_catalogue.AddBusToStop(stop, key_bus->first);
            rout_b.push_back(key_stop->first);
        }
        transport_catalogue.AddBusRoute(quere.name, is_circle, rout_b);
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

istream& operator>>(istream& is, InputReader::InputReader& reader) {
    string line;
    getline(is, line);
    size_t space = line.find_first_of(' ');
    size_t point = line.find_first_of(':');
    InputReader::Quere::Quere quere;
    quere.type = line.substr(0, space);
    quere.name = line.substr(space + 1, point - space - 1);
    quere.data = line.substr(point + 2, line.size() - point);
    reader.AddQuere(move(quere));
    return is;
}
