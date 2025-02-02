#include "Algorithms.hpp"
#include <algorithm>
#include "Utilities.hpp"

double calculateChangeMultiplier(const StopDict& stopDict)
{
    auto [record1, result1] = dijkstra("Trzebnicka","DH Astra", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist1 = Utilities::euclidianDistance(stopDict.at("Trzebnicka").latitude, stopDict.at("Trzebnicka").longitude, stopDict.at("DH Astra").latitude, stopDict.at("DH Astra").longitude);

    auto [record2, result2] = dijkstra("Wiśniowa","PL. GRUNWALDZKI", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist2 = Utilities::euclidianDistance(stopDict.at("Wiśniowa").latitude, stopDict.at("Wiśniowa").longitude, stopDict.at("PL. GRUNWALDZKI").latitude, stopDict.at("PL. GRUNWALDZKI").longitude);
    
    auto [record3, result3] = dijkstra("Paprotna","DWORZEC GŁÓWNY", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist3 = Utilities::euclidianDistance(stopDict.at("Paprotna").latitude, stopDict.at("Paprotna").longitude, stopDict.at("DWORZEC GŁÓWNY").latitude, stopDict.at("DWORZEC GŁÓWNY").longitude);

    auto [record4, result4] = dijkstra("C.H. Korona","FAT", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist4 = Utilities::euclidianDistance(stopDict.at("C.H. Korona").latitude, stopDict.at("C.H. Korona").longitude, stopDict.at("FAT").latitude, stopDict.at("FAT").longitude);

    auto [record5, result5] = dijkstra("Jedności Narodowej","Rondo", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist5 = Utilities::euclidianDistance(stopDict.at("Jedności Narodowej").latitude, stopDict.at("Jedności Narodowej").longitude, stopDict.at("Rondo").latitude, stopDict.at("Rondo").longitude);

    auto [record6, result6] = dijkstra("KRAKOWSKA (Centrum handlowe)","GALERIA DOMINIKAŃSKA", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist6 = Utilities::euclidianDistance(stopDict.at("KRAKOWSKA (Centrum handlowe)").latitude, stopDict.at("KRAKOWSKA (Centrum handlowe)").longitude, stopDict.at("GALERIA DOMINIKAŃSKA").latitude, stopDict.at("GALERIA DOMINIKAŃSKA").longitude);

    auto [record7, result7] = dijkstra("Prusa","Nowowiejska", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist7 = Utilities::euclidianDistance(stopDict.at("Prusa").latitude, stopDict.at("Prusa").longitude, stopDict.at("Nowowiejska").latitude, stopDict.at("Nowowiejska").longitude);

    auto [record8, result8] = dijkstra("SĘPOLNO","PL. GRUNWALDZKI", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist8 = Utilities::euclidianDistance(stopDict.at("SĘPOLNO").latitude, stopDict.at("SĘPOLNO").longitude, stopDict.at("PL. GRUNWALDZKI").latitude, stopDict.at("PL. GRUNWALDZKI").longitude);

    auto [record9, result9] = dijkstra("Ogród Botaniczny","Dubois", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist9 = Utilities::euclidianDistance(stopDict.at("Ogród Botaniczny").latitude, stopDict.at("Ogród Botaniczny").longitude, stopDict.at("Dubois").latitude, stopDict.at("Dubois").longitude);

    auto [record10, result10] = dijkstra("DH Astra","LEŚNICA", Utilities::makeTimeFromStr("10:00:00"), stopDict, OptimizationType::ChangeCount);
    auto dist10 = Utilities::euclidianDistance(stopDict.at("DH Astra").latitude, stopDict.at("DH Astra").longitude, stopDict.at("LEŚNICA").latitude, stopDict.at("LEŚNICA").longitude);

    std::vector<double> changeMultipliers{result1/dist1, result2/dist2, result3/dist3, result4/dist4, result5/dist5, result6/dist6, result7/dist7, result8/dist8, result9/dist9, result10/dist10};

    return *std::min_element(changeMultipliers.begin(), changeMultipliers.end());
}

std::pair<std::vector<RouteRecord>,double> dijkstra(const std::string& startStop, const std::string& endStop, time_t startTime, const StopDict& stopDict, OptimizationType type)
{
    D d;
    Q Q;
    RouteRecordDict routeRecords;
    auto comp = [&d](const std::string& s1, const std::string& s2) {
        return d.at(s1) < d.at(s2);
    };

    for(const auto& stop: stopDict)
    {
        d.emplace(stop.first,std::numeric_limits<double>::max());
        Q.emplace(stop.first);
        routeRecords.emplace(stop.first,std::vector<RouteRecord>{});    
    }
    d.at(startStop) = 0;

    routeRecords.at(startStop).push_back({.line=std::string{},.departureTime=time_t{},.arrivalTime=startTime,.startName=std::string{},.stopName=startStop});
    time_t currTime;
    std::string currLine;

    while(not Q.empty())
    {
        const auto smallest = *std::min_element(Q.cbegin(), Q.cend(), comp);
        Q.erase(smallest);
        // std::cout<<"analysing: "<<smallest<<std::endl;
        // std::cout<<Q.size()<<" to go"<<std::endl;

        const auto& neighborhood = stopDict.at(smallest).neighbours;
        // std::cout<<"neighbor list taken"<<std::endl;

        if(routeRecords.at(smallest).empty())
            continue; // for stops that cant be reached

        currTime = routeRecords.at(smallest).back().arrivalTime;
        // std::cout<<"current time :";
        // Utilities::toStream(std::cout,currTime);
        // std::cout<<std::endl;
        currLine = routeRecords.at(smallest).back().line;
        // std::cout<<"current line: "<<currLine<<std::endl;

        for(const auto& neighbor: neighborhood)
        {
            // std::cout<<"\t\tcurrent neighbor: "<<neighbor.first<<" " <<std::flush;
            auto [edge, weight] = chooseBestEdge(currTime, type, stopDict.at(smallest), neighbor.first, currLine);
            // std::cout<<"best edge found! "<<edge<<std::endl;
            if(weight + d.at(smallest) < d.at(neighbor.first))
            {
                // std::cout<<"new edge chosen!"<<std::endl;
                d.at(neighbor.first) = weight + d.at(smallest);
                routeRecords.at(neighbor.first) = routeRecords.at(smallest);
                if(routeRecords.at(neighbor.first).back().line == edge.line)
                {
                    routeRecords.at(neighbor.first).back().arrivalTime = edge.arrivalTime;
                    routeRecords.at(neighbor.first).back().stopName = neighbor.first;
                }
                else
                {
                    routeRecords.at(neighbor.first).push_back({.line=edge.line, .departureTime=edge.departureTime, .arrivalTime=edge.arrivalTime, .startName=smallest, .stopName=neighbor.first});
                }
            }
        }

    }
    return {routeRecords.at(endStop), d.at(endStop)};
    
}

std::pair<std::vector<RouteRecord>,double> aStar(const std::string& startStop, const std::string& endStop, time_t startTime, const StopDict& stopDict, OptimizationType type, double multiplier, int& visitedStops)
{
    F F;
    G G;
    H H;
    Open open;
    Closed closed;
    std::string currStop;
    RouteRecordDict routeRecords;

    for(const auto& stop: stopDict)
    {
        F.emplace(stop.first, std::numeric_limits<double>::max());
        G.emplace(stop.first, std::numeric_limits<double>::max());
        H.emplace(stop.first, std::numeric_limits<double>::max());
        routeRecords.emplace(stop.first,std::vector<RouteRecord>{}); 
    }
    F.at(startStop) = 0;
    G.at(startStop) = 0;
    H.at(startStop) = 0;
    open.emplace(startStop);
    routeRecords.at(startStop).push_back({.line=std::string{},.departureTime=time_t{},.arrivalTime=startTime,.startName=std::string{},.stopName=startStop});
    time_t currTime;
    std::string currLine;

    // std::cout<<"initializing done"<<std::endl;

    auto comp = [&F](const std::string& s1, const std::string& s2) {
        return F.at(s1) < F.at(s2);
    };

    // int i{};
    // std::cout<<"counter set"<<std::endl;

    while(not open.empty())
    {
        // i++;
        visitedStops++;
        currStop = *std::min_element(open.cbegin(), open.cend(), comp);

        if(currStop == endStop)
        {
            // std::cout<<"stops visited: "<<i<<std::endl;
            return {routeRecords.at(endStop), G.at(endStop)};
        }

        // std::cout<<"analysing: "<<currStop<<std::endl;

        open.erase(currStop);
        closed.emplace(currStop);

        const auto& neighborhood = stopDict.at(currStop).neighbours;
        // std::cout<<"neighbor list taken"<<std::endl;

        currTime = routeRecords.at(currStop).back().arrivalTime;
        // std::cout<<"current time :";
        // Utilities::toStream(std::cout,currTime);
        // std::cout<<std::endl;
        currLine = routeRecords.at(currStop).back().line;
        // std::cout<<"current line: "<<currLine<<std::endl;

        for(const auto& neighbor: neighborhood)
        {
            // std::cout<<"\t\tcurrent neighbor: "<<neighbor.first<<" " <<std::flush;
            auto [edge, weight] = chooseBestEdge(currTime, type, stopDict.at(currStop), neighbor.first, currLine);
            // std::cout<<"best edge found! "<<edge<<std::endl;

            if(open.find(neighbor.first) == open.end() and closed.find(neighbor.first) == closed.end())
            {
                open.emplace(neighbor.first);

                H.at(neighbor.first) = calculateCostEstimationBetweenNextNodeAndEndNode(stopDict.at(currStop), stopDict.at(neighbor.first), multiplier);
                G.at(neighbor.first) = weight + G.at(currStop);
                F.at(neighbor.first) = H.at(neighbor.first) + G.at(neighbor.first);

                routeRecords.at(neighbor.first) = routeRecords.at(currStop);
                if(routeRecords.at(neighbor.first).back().line == edge.line)
                {
                    routeRecords.at(neighbor.first).back().arrivalTime = edge.arrivalTime;
                    routeRecords.at(neighbor.first).back().stopName = neighbor.first;
                }
                else
                {
                    routeRecords.at(neighbor.first).push_back({.line=edge.line, .departureTime=edge.departureTime, .arrivalTime=edge.arrivalTime, .startName=currStop, .stopName=neighbor.first});
                }
            }
            else
            {
                if(weight + G.at(currStop) < G.at(neighbor.first))
                {
                    G.at(neighbor.first) = weight + G.at(currStop);
                    F.at(neighbor.first) = H.at(neighbor.first) + G.at(neighbor.first);
                    if(closed.find(neighbor.first) != closed.end())
                    {
                        open.emplace(neighbor.first);
                        closed.erase(neighbor.first);
                    }
                    routeRecords.at(neighbor.first) = routeRecords.at(currStop);
                    if(routeRecords.at(neighbor.first).back().line == edge.line)
                    {
                        routeRecords.at(neighbor.first).back().arrivalTime = edge.arrivalTime;
                        routeRecords.at(neighbor.first).back().stopName = neighbor.first;
                    }
                    else
                    {
                        routeRecords.at(neighbor.first).push_back({.line=edge.line, .departureTime=edge.departureTime, .arrivalTime=edge.arrivalTime, .startName=currStop, .stopName=neighbor.first});
                    }

                }
            }
        }
        // std::cout<<"analysing end: "<<currStop<<std::endl;
    }

    return {routeRecords.at(endStop), G.at(endStop)};
}

std::pair<std::vector<RouteRecord>,double> aStarChangeOptimization(const std::string& startStop, const std::string& endStop, time_t startTime, const StopDict& stopDict, double multiplier, const LineStopSets& stopSets, int& visitedStops)
{
    F F;
    G G;
    H H;
    Open open;
    Closed closed;
    std::string currStop;
    RouteRecordDict routeRecords;

    for(const auto& stop: stopDict)
    {
        F.emplace(stop.first, std::numeric_limits<double>::max());
        G.emplace(stop.first, std::numeric_limits<double>::max());
        H.emplace(stop.first, std::numeric_limits<double>::max());
        routeRecords.emplace(stop.first,std::vector<RouteRecord>{}); 
    }
    F.at(startStop) = 0;
    G.at(startStop) = 0;
    H.at(startStop) = 0;
    open.emplace(startStop);
    routeRecords.at(startStop).push_back({.line=std::string{},.departureTime=time_t{},.arrivalTime=startTime,.startName=std::string{},.stopName=startStop});
    time_t currTime;
    std::string currLine;

    // std::cout<<"initializing done"<<std::endl;

    auto comp = [&F](const std::string& s1, const std::string& s2) {
        return F.at(s1) < F.at(s2);
    };

    // int i{};
    // std::cout<<"counter set"<<std::endl;

    while(not open.empty())
    {
        // i++;
        visitedStops++;
        currStop = *std::min_element(open.cbegin(), open.cend(), comp);

        if(currStop == endStop)
        {
            // std::cout<<"stops visited: "<<i<<std::endl;
            return {routeRecords.at(endStop), G.at(endStop)};
        }

        // std::cout<<"analysing: "<<currStop<<std::endl;

        open.erase(currStop);
        closed.emplace(currStop);

        const auto& neighborhood = stopDict.at(currStop).neighbours;
        // std::cout<<"neighbor list taken"<<std::endl;

        currTime = routeRecords.at(currStop).back().arrivalTime;
        // std::cout<<"current time :";
        // Utilities::toStream(std::cout,currTime);
        // std::cout<<std::endl;
        currLine = routeRecords.at(currStop).back().line;
        // std::cout<<"current line: "<<currLine<<std::endl;

        for(const auto& neighbor: neighborhood)
        {
            // std::cout<<"\t\tcurrent neighbor: "<<neighbor.first<<" " <<std::flush;
            auto [edge, weight] = chooseBestEdgeDirectLineOptimization(currTime, stopDict.at(currStop), neighbor.first, currLine, endStop, stopSets);
            // std::cout<<"best edge found! "<<edge<<std::endl;

            if(open.find(neighbor.first) == open.end() and closed.find(neighbor.first) == closed.end())
            {
                open.emplace(neighbor.first);

                H.at(neighbor.first) = calculateCostEstimationBetweenNextNodeAndEndNode(stopDict.at(currStop), stopDict.at(neighbor.first), multiplier);
                G.at(neighbor.first) = weight + G.at(currStop);
                F.at(neighbor.first) = H.at(neighbor.first) + G.at(neighbor.first);

                routeRecords.at(neighbor.first) = routeRecords.at(currStop);
                if(routeRecords.at(neighbor.first).back().line == edge.line)
                {
                    routeRecords.at(neighbor.first).back().arrivalTime = edge.arrivalTime;
                    routeRecords.at(neighbor.first).back().stopName = neighbor.first;
                }
                else
                {
                    routeRecords.at(neighbor.first).push_back({.line=edge.line, .departureTime=edge.departureTime, .arrivalTime=edge.arrivalTime, .startName=currStop, .stopName=neighbor.first});
                }
            }
            else
            {
                if(weight + G.at(currStop) < G.at(neighbor.first))
                {
                    G.at(neighbor.first) = weight + G.at(currStop);
                    F.at(neighbor.first) = H.at(neighbor.first) + G.at(neighbor.first);
                    if(closed.find(neighbor.first) != closed.end())
                    {
                        open.emplace(neighbor.first);
                        closed.erase(neighbor.first);
                    }
                    routeRecords.at(neighbor.first) = routeRecords.at(currStop);
                    if(routeRecords.at(neighbor.first).back().line == edge.line)
                    {
                        routeRecords.at(neighbor.first).back().arrivalTime = edge.arrivalTime;
                        routeRecords.at(neighbor.first).back().stopName = neighbor.first;
                    }
                    else
                    {
                        routeRecords.at(neighbor.first).push_back({.line=edge.line, .departureTime=edge.departureTime, .arrivalTime=edge.arrivalTime, .startName=currStop, .stopName=neighbor.first});
                    }

                }
            }
        }
        // std::cout<<"analysing end: "<<currStop<<std::endl;
    }

    return {routeRecords.at(endStop), G.at(endStop)};
}