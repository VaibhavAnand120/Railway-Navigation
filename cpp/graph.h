#pragma once
#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <climits>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip>

using namespace std;

struct Edge {
    int to;
    int distance;  // km
    int duration;  // minutes
    double fare;   // base fare in INR
    string trainId;
};

struct Station {
    int id;
    string code;
    string name;
    string city;
    string zone;
};

struct Route {
    vector<int> path;
    int totalDistance;
    int totalDuration;
    double totalFare;
    vector<string> trains;
};

struct TrainSchedule {
    string trainId;
    string trainName;
    int fromStation;
    int toStation;
    string departure;
    string arrival;
    int distance;
    double fare;
    int availableSeats;
};

class RailGraph {
public:
    unordered_map<int, Station> stations;
    unordered_map<string, int> stationCodeToId;
    unordered_map<string, int> stationNameToId;
    vector<vector<Edge>> adj;
    unordered_map<string, TrainSchedule> schedules;
    int numStations;

    RailGraph();
    void addStation(int id, const string& code, const string& name, const string& city, const string& zone);
    void addEdge(int from, int to, int distance, int duration, double fare, const string& trainId);
    void addSchedule(const TrainSchedule& schedule);
    
    // Core algorithms
    Route dijkstra(int src, int dest);
    vector<Route> bfsKPaths(int src, int dest, int k);
    vector<int> bfsNearbyStations(int src, int maxDistance);
    
    // Utility
    int getStationId(const string& nameOrCode);
    void loadFromCSV(const string& stationsFile, const string& edgesFile);
    void logRoute(const Route& route, const string& fromName, const string& toName);
    double calculateFare(int distance, const string& travelClass);
    void printRoute(const Route& route);
};
