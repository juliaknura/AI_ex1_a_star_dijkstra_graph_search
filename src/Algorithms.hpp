#pragma once
#include "DataStructures.hpp"
#include <vector>
#include <limits>
#include <iostream>

double calculateChangeMultiplier(const StopDict& stopDist);

std::pair<std::vector<RouteRecord>,double> dijkstra(const std::string& startStop, const std::string& endStop, time_t currTime, const StopDict& stopDict, OptimizationType type);

std::pair<std::vector<RouteRecord>,double> aStar(const std::string& startStop, const std::string& endStop, time_t currTime, const StopDict& stopDict, OptimizationType type, double multiplier, int& visitedStops);

std::pair<std::vector<RouteRecord>,double> aStarChangeOptimization(const std::string& startStop, const std::string& endStop, time_t currTime, const StopDict& stopDict, double multiplier, const LineStopSets& stopSets, int& visitedStops);