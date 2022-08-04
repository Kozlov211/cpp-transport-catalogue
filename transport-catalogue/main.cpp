#include <algorithm>
#include <sstream>
#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"


using namespace std;

int main() {
    TransportCatalogue::TransportCatalogue transport_catalogue;
    InputReader::InputReader input_reader(transport_catalogue);
    StatReader::StatReader stat_reader(transport_catalogue);
    size_t query_count;
    cin >> query_count;
    InputReader::ReadLine();
    input_reader.DataInput(query_count);
    cin >> query_count;
    InputReader::ReadLine();
    for (size_t i = 0; i  < query_count; ++i) {
        cin >> stat_reader;
        cout << stat_reader;
    }
}


