#pragma once
#include <cstdio>
#include <ctime>
#include <string>
#include <vector>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <set>
#include <iostream>

enum class OptimizationType
{
    ChangeCount,Time
};

struct Stop;

struct Edge {
    std::string line;
    time_t departureTime;
    time_t arrivalTime;
};

std::ostream& operator<<(std::ostream& os,const Edge& edge);

struct EdgeComparator{
    bool operator()(const Edge& lhs, const Edge& rhs) const
    {
        return lhs.departureTime < rhs.departureTime;
    }
};

using NeighborMap = std::unordered_map<std::string, std::set<Edge,EdgeComparator>>;

struct Stop{
    std::string name;
    double latitude;
    double longitude;
    NeighborMap neighbours;
};

std::ostream& operator<<(std::ostream& os,const Stop& edge);

struct RouteRecord {
    std::string line;
    time_t departureTime;
    time_t arrivalTime;
    std::string startName;
    std::string stopName;
};

std::ostream& operator<<(std::ostream& os,const RouteRecord& edge);

using D = std::unordered_map<std::string, double>;
using F = std::unordered_map<std::string, double>;
using H = std::unordered_map<std::string, double>;
using G = std::unordered_map<std::string, double>;
using StopDict = std::unordered_map<std::string, Stop>;
using RouteRecordDict = std::unordered_map<std::string, std::vector<RouteRecord>>;
using Open = std::unordered_set<std::string>;
using Closed = std::unordered_set<std::string>;
using Q = std::unordered_set<std::string>;
using LineStopSets = std::unordered_map<std::string, std::unordered_set<std::string>>;


std::pair<const Edge&,int> chooseBestEdge(time_t currTime, OptimizationType type, const Stop& curr, const std::string& next, const std::string& prevLine);

double calculateCostEstimationBetweenNextNodeAndEndNode(const Stop& next, const Stop& dest, double multiplier);

std::pair<const Edge&,int> chooseBestEdgeDirectLineOptimization(time_t currTime, const Stop& curr, const std::string& next, const std::string& prevLine, const std::string& end, const LineStopSets& stopSets);