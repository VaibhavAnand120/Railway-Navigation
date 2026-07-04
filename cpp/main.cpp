#include "graph.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "{\"error\": \"No command provided\"}" << endl;
        return 1;
    }

    RailGraph g;
    g.loadFromCSV("stations.csv", "edges.csv");

    string command = argv[1];

    if (command == "route" && argc >= 4) {
        string from = argv[2];
        string to   = argv[3];
        string travelClass = (argc >= 5) ? argv[4] : "SL";

        int srcId  = g.getStationId(from);
        int destId = g.getStationId(to);

        if (srcId == -1 || destId == -1) {
            cout << "{\"error\": \"Station not found\"}" << endl;
            return 1;
        }

        Route route = g.dijkstra(srcId, destId);
        route.totalFare = g.calculateFare(route.totalDistance, travelClass);
        g.logRoute(route, from, to);
        g.printRoute(route);
    }
    else if (command == "nearby" && argc >= 4) {
        string station = argv[2];
        int maxDist    = atoi(argv[3]);
        int srcId      = g.getStationId(station);

        if (srcId == -1) {
            cout << "{\"error\": \"Station not found\"}" << endl;
            return 1;
        }

        vector<int> nearby = g.bfsNearbyStations(srcId, maxDist);
        cout << "{\"nearby\": [";
        for (int i = 0; i < (int)nearby.size(); i++) {
            cout << "\"" << g.stations[nearby[i]].name << "\"";
            if (i < (int)nearby.size() - 1) cout << ",";
        }
        cout << "]}";
    }
    else if (command == "alternatives" && argc >= 5) {
        string from = argv[2];
        string to   = argv[3];
        int k       = atoi(argv[4]);

        int srcId  = g.getStationId(from);
        int destId = g.getStationId(to);

        if (srcId == -1 || destId == -1) {
            cout << "{\"error\": \"Station not found\"}" << endl;
            return 1;
        }

        vector<Route> routes = g.bfsKPaths(srcId, destId, k);
        cout << "{\"routes\": [";
        for (int i = 0; i < (int)routes.size(); i++) {
            g.printRoute(routes[i]);
            if (i < (int)routes.size() - 1) cout << ",";
        }
        cout << "]}";
    }
    else if (command == "fare" && argc >= 4) {
        int distance       = atoi(argv[2]);
        string travelClass = argv[3];
        double fare        = g.calculateFare(distance, travelClass);
        cout << "{\"fare\": " << fare << ", \"class\": \"" << travelClass << "\", \"distance\": " << distance << "}";
    }
    else if (command == "stations") {
        cout << "{\"stations\": [";
        bool first = true;
        for (auto it = g.stations.begin(); it != g.stations.end(); ++it) {
            Station& s = it->second;
            if (!first) cout << ",";
            cout << "{\"id\":" << s.id << ",\"code\":\"" << s.code
                 << "\",\"name\":\"" << s.name << "\",\"city\":\"" << s.city
                 << "\",\"zone\":\"" << s.zone << "\"}";
            first = false;
        }
        cout << "]}";
    }
    else {
        cout << "{\"error\": \"Unknown command\"}" << endl;
        return 1;
    }

    return 0;
}
