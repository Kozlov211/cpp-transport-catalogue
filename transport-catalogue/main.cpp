#include <iostream>


#include "map_renderer.h"
#include "json_reader.h"
#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"

using namespace std;

int main() {
    TransportCatalogue::TransportCatalogue transport_catalogue;
    JsonReader::JsonReader json_reader(transport_catalogue);
    json_reader.ReadInputData(cin);
    MapRenderer::MapRenderer map_renderer;
    RequestHandler::RequestHandler request_handler(transport_catalogue, map_renderer);
    request_handler.AppendRequests(json_reader);
    request_handler.AppendGeoCoordinatesToMapRender();
    request_handler.AppendBusesToMapRender();
    map_renderer.AppendRenderSettings(json_reader);
    cout << request_handler.GetResponseToRequest() << endl;
}

