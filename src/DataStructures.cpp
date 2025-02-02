#include "DataStructures.hpp"
#include "Utilities.hpp"
#include <algorithm>

std::pair<const Edge&,int> chooseBestEdge(time_t currTime, OptimizationType type, const Stop& curr, const std::string& next, const std::string& prevLine)
{
    int bestWeight{};
    std::set<Edge, EdgeComparator>::iterator bestEdge;
    const auto& edgeList = curr.neighbours.at(next);
    auto leavesNoSoonerThanCurrentTime = [currTime=currTime](const Edge& edge) {return edge.departureTime >= currTime;};

    auto edgeNoSoonerThanCurrTime = std::find_if(edgeList.cbegin(), edgeList.cend(),leavesNoSoonerThanCurrentTime);
    if(edgeNoSoonerThanCurrTime == edgeList.end())
        edgeNoSoonerThanCurrTime = edgeList.begin();

    if(type==OptimizationType::Time)
    {
        bestEdge = edgeNoSoonerThanCurrTime;
        bestWeight = Utilities::timeDiff(bestEdge->arrivalTime,currTime);

        auto searchEndTime = bestEdge->arrivalTime;
        auto currEdge = bestEdge;
        currEdge++;
        if(currEdge==edgeList.end())
            currEdge = edgeList.begin();

        int loopCount{}; // guard against endless loop

        while(currEdge->departureTime < searchEndTime && loopCount <= edgeList.size())
        {
            if(Utilities::timeDiff(currEdge->arrivalTime,currTime) < bestWeight)
            {
                bestWeight = Utilities::timeDiff(currEdge->arrivalTime,currTime);
                bestEdge = currEdge;
            }

            currEdge++;
            if(currEdge==edgeList.end())
                currEdge = edgeList.begin();

            loopCount++;
        }
    }
    else
    {
        auto sameLine = [line=prevLine](const Edge& edge){return edge.line == line;};
        
        auto edgeSameLine = std::find_if(edgeNoSoonerThanCurrTime, edgeList.cend(),sameLine);
        if(edgeSameLine == edgeList.end())
        {
            edgeSameLine = std::find_if(edgeList.begin(), edgeNoSoonerThanCurrTime,sameLine);
            if(edgeSameLine == edgeNoSoonerThanCurrTime)
            {
                bestEdge = edgeNoSoonerThanCurrTime;
                bestWeight = 1;
            }
            else
            {
                bestEdge = edgeSameLine;
                bestWeight = 0;
            }
        }
        else
        {
            bestEdge = edgeSameLine;
            bestWeight = 0;
        }
    }
    return {*bestEdge,bestWeight};
}

std::pair<const Edge&,int> chooseBestEdgeDirectLineOptimization(time_t currTime, const Stop& curr, const std::string& next, const std::string& prevLine, const std::string& end, const LineStopSets& stopSets)
{
    int bestWeight{};
    std::set<Edge, EdgeComparator>::iterator bestEdge;
    const auto& edgeList = curr.neighbours.at(next);
    auto leavesNoSoonerThanCurrentTime = [currTime=currTime](const Edge& edge) {return edge.departureTime >= currTime;};

    auto edgeNoSoonerThanCurrTime = std::find_if(edgeList.cbegin(), edgeList.cend(),leavesNoSoonerThanCurrentTime);
    if(edgeNoSoonerThanCurrTime == edgeList.end())
        edgeNoSoonerThanCurrTime = edgeList.begin();

    if(prevLine.empty()) //checks if there exists a direct line
    {
        std::unordered_set<std::string> lines;
        std::unordered_set<std::string> directLines;
        for(const auto& edge: edgeList)
            lines.emplace(edge.line);
        for(const auto& line: lines)
        {
            if(stopSets.at(line).count(curr.name) and stopSets.at(line).count(end))
                directLines.emplace(line);
        }

        if(not directLines.empty())
        {
            auto firstOfDirectLines = [&directLines](const Edge& edge) {return directLines.count(edge.line) > 0;};
            bestEdge = std::find_if(edgeNoSoonerThanCurrTime, edgeList.cend(), firstOfDirectLines);
            if(bestEdge == edgeList.end())
                bestEdge = std::find_if(edgeList.cbegin(), edgeNoSoonerThanCurrTime, firstOfDirectLines);
            
            return {*bestEdge,1};
        }
        else
            return {*edgeNoSoonerThanCurrTime,1};
    }
    auto sameLine = [line=prevLine](const Edge& edge){return edge.line == line;};
        
    auto edgeSameLine = std::find_if(edgeNoSoonerThanCurrTime, edgeList.cend(),sameLine);
    if(edgeSameLine == edgeList.end())
    {
        edgeSameLine = std::find_if(edgeList.begin(), edgeNoSoonerThanCurrTime,sameLine);
        if(edgeSameLine == edgeNoSoonerThanCurrTime)
        {
            bestEdge = edgeNoSoonerThanCurrTime;
            bestWeight = 1;
        }
        else
        {
            bestEdge = edgeSameLine;
            bestWeight = 0;
        }
    }
    else
    {
        bestEdge = edgeSameLine;
        bestWeight = 0;
    }

    return {*bestEdge,bestWeight};
}

double calculateCostEstimationBetweenNextNodeAndEndNode(const Stop& next, const Stop& dest, double multiplier)
{
    return multiplier*(std::abs(next.latitude - dest.latitude) + std::abs(next.longitude - dest.longitude));
}

std::ostream& operator<<(std::ostream& os,const Edge& edge)
{
    os<<"line: "<<edge.line<<", departure: ";
    Utilities::toStream(os,edge.departureTime);
    os<<", arrival: ";
    Utilities::toStream(os,edge.arrivalTime);
    return os;
}

std::ostream& operator<<(std::ostream& os,const Stop& stop)
{
    os<<"name: "<<stop.name<<", latitude: "<<stop.latitude<<", longitude: "<<stop.longitude;
    return os;
}

std::ostream& operator<<(std::ostream& os,const RouteRecord& record)
{
    os<<"line: "<<record.line<<", departureTime: ";
    Utilities::toStream(os,record.departureTime);
    os<<", arrivalTime: ";
    Utilities::toStream(os,record.arrivalTime);
    os<<", startName: "<<record.startName<<", stopName: "<<record.stopName;
    return os;  
}