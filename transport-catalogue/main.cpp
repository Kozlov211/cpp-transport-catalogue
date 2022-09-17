#include <iostream>
#include <sstream>

#include "map_renderer.h"
#include "json_reader.h"
#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"

using namespace std;

using namespace transport_catalogue;
using namespace json_reader;
using namespace map_renderer;

int main() {
    TransportCatalogue transport_catalogue;
    JsonReader json_reader(transport_catalogue);
    json_reader.ReadJson(cin);
    MapRenderer map_renderer;
    RequestHandler::RequestHandler request_handler(transport_catalogue, map_renderer);
    request_handler.AppendBusesToMapRender();
    request_handler.AppendGeoCoordinatesToMapRender();
    request_handler.AppendBusesToMapRender();
    cout << request_handler.GetResponseToRequest(json_reader) << endl;
}
