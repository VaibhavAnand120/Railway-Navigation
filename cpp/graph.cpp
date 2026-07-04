#include "graph.h"
#include <cmath>

RailGraph::RailGraph() : numStations(0) {
    adj.resize(500);
}

void RailGraph::addStation(int id, const string& code, const string& name, const string& city, const string& zone) {
    Station s = {id, code, name, city, zone};
    stations[id] = s;
    stationCodeToId[code] = id;
    stationNameToId[name] = id;
    if (id + 1 > numStations) numStations = id + 1;
}

void RailGraph::addEdge(int from, int to, int distance, int duration, double fare, const string& trainId) {
    adj[from].push_back({to, distance, duration, fare, trainId});
    adj[to].push_back({from, distance, duration, fare, trainId});
}

void RailGraph::addSchedule(const TrainSchedule& schedule) {
    schedules[schedule.trainId] = schedule;
}

Route RailGraph::dijkstra(int src, int dest) {
    vector<int> dist(numStations, INT_MAX);
    vector<int> prev(numStations, -1);
    vector<string> prevTrain(numStations, "");
    vector<int> duration(numStations, 0);
    vector<double> fare(numStations, 0.0);

    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    dist[src] = 0;
    pq.push(make_pair(0, src));

    while (!pq.empty()) {
        int d = pq.top().first;
        int u = pq.top().second;
        pq.pop();
        if (d > dist[u]) continue;

        for (int i = 0; i < (int)adj[u].size(); i++) {
            Edge& e = adj[u][i];
            if (dist[u] + e.distance < dist[e.to]) {
                dist[e.to] = dist[u] + e.distance;
                prev[e.to] = u;
                prevTrain[e.to] = e.trainId;
                duration[e.to] = duration[u] + e.duration;
                fare[e.to] = fare[u] + e.fare;
                pq.push(make_pair(dist[e.to], e.to));
            }
        }
    }

    Route route;
    route.totalDistance = dist[dest];
    route.totalDuration = duration[dest];
    route.totalFare = fare[dest];

    if (dist[dest] == INT_MAX) return route;

    for (int at = dest; at != -1; at = prev[at]) {
        route.path.push_back(at);
        if (!prevTrain[at].empty()) route.trains.push_back(prevTrain[at]);
    }
    reverse(route.path.begin(), route.path.end());
    reverse(route.trains.begin(), route.trains.end());
    return route;
}

vector<Route> RailGraph::bfsKPaths(int src, int dest, int k) {
    vector<Route> result;
    queue<pair<vector<int>, int>> q;
    vector<int> initPath;
    initPath.push_back(src);
    q.push(make_pair(initPath, 0));

    while (!q.empty() && (int)result.size() < k) {
        vector<int> path = q.front().first;
        int totalDist    = q.front().second;
        q.pop();
        int curr = path.back();

        if (curr == dest) {
            Route r;
            r.path = path;
            r.totalDistance = totalDist;
            r.totalFare = calculateFare(totalDist, "SL");
            result.push_back(r);
            continue;
        }

        for (int i = 0; i < (int)adj[curr].size(); i++) {
            Edge& e = adj[curr][i];
            bool visited = false;
            for (int j = 0; j < (int)path.size(); j++) {
                if (path[j] == e.to) { visited = true; break; }
            }
            if (!visited) {
                vector<int> newPath = path;
                newPath.push_back(e.to);
                q.push(make_pair(newPath, totalDist + e.distance));
            }
        }
    }
    return result;
}

vector<int> RailGraph::bfsNearbyStations(int src, int maxDistance) {
    vector<int> nearby;
    vector<int> dist(numStations, INT_MAX);
    queue<int> q;
    dist[src] = 0;
    q.push(src);

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int i = 0; i < (int)adj[u].size(); i++) {
            Edge& e = adj[u][i];
            if (dist[u] + e.distance <= maxDistance && dist[e.to] == INT_MAX) {
                dist[e.to] = dist[u] + e.distance;
                nearby.push_back(e.to);
                q.push(e.to);
            }
        }
    }
    return nearby;
}

int RailGraph::getStationId(const string& nameOrCode) {
    if (stationCodeToId.count(nameOrCode)) return stationCodeToId[nameOrCode];
    if (stationNameToId.count(nameOrCode)) return stationNameToId[nameOrCode];
    return -1;
}

double RailGraph::calculateFare(int distance, const string& travelClass) {
    double baseFare = 0.0;
    if (travelClass == "SL")      baseFare = distance * 0.5 + 30;
    else if (travelClass == "3A") baseFare = distance * 1.2 + 100;
    else if (travelClass == "2A") baseFare = distance * 1.8 + 150;
    else if (travelClass == "1A") baseFare = distance * 3.0 + 250;
    else if (travelClass == "CC") baseFare = distance * 0.9 + 50;
    else                          baseFare = distance * 0.4 + 20;
    return floor(baseFare + 0.5);
}

void RailGraph::loadFromCSV(const string& stationsFile, const string& edgesFile) {
    ifstream sf(stationsFile);
    string line;
    getline(sf, line);
    while (getline(sf, line)) {
        stringstream ss(line);
        string id, code, name, city, zone;
        getline(ss, id, ',');
        getline(ss, code, ',');
        getline(ss, name, ',');
        getline(ss, city, ',');
        getline(ss, zone, ',');
        if (!id.empty()) addStation(stoi(id), code, name, city, zone);
    }

    ifstream ef(edgesFile);
    getline(ef, line);
    while (getline(ef, line)) {
        stringstream ss(line);
        string from, to, dist, dur, fare, trainId;
        getline(ss, from, ',');
        getline(ss, to, ',');
        getline(ss, dist, ',');
        getline(ss, dur, ',');
        getline(ss, fare, ',');
        getline(ss, trainId, ',');
        if (!from.empty()) addEdge(stoi(from), stoi(to), stoi(dist), stoi(dur), stod(fare), trainId);
    }
}

void RailGraph::logRoute(const Route& route, const string& fromName, const string& toName) {
    ofstream log("route_history.log", ios::app);
    log << fromName << " -> " << toName
        << " | Distance: " << route.totalDistance << "km"
        << " | Duration: " << route.totalDuration << "min"
        << " | Fare: INR " << route.totalFare << "\n";
}

void RailGraph::printRoute(const Route& route) {
    if (route.path.empty()) {
        cout << "{\"error\": \"No route found\"}" << endl;
        return;
    }
    cout << "{";
    cout << "\"distance\": " << route.totalDistance << ",";
    cout << "\"duration\": " << route.totalDuration << ",";
    cout << "\"fare\": " << route.totalFare << ",";
    cout << "\"path\": [";
    for (int i = 0; i < (int)route.path.size(); i++) {
        int id = route.path[i];
        cout << "\"" << stations[id].name << "\"";
        if (i < (int)route.path.size() - 1) cout << ",";
    }
    cout << "],";
    cout << "\"trains\": [";
    for (int i = 0; i < (int)route.trains.size(); i++) {
        cout << "\"" << route.trains[i] << "\"";
        if (i < (int)route.trains.size() - 1) cout << ",";
    }
    cout << "]}";
}
