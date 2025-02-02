#pragma once
#include <string>
#include "DataStructures.hpp"

double loadData(const std::string& fileName, StopDict& stopDict);

double loadData2(const std::string& fileName, StopDict& stopDict, LineStopSets& stopSets, std::vector<std::string>& stops);