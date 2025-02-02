#include "DataLoader.hpp"
#include "Utilities.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <numeric>

double loadData(const std::string& fileName, StopDict& stopDict)
{
    double timeMult = std::numeric_limits<double>::infinity();
    std::ifstream file(fileName);
    std::string line;
    getline(file,line);
    while(getline(file,line))
    {
        std::stringstream ss{line};
        getline(ss,line,',');
        std::string company, line, departure, arrival, start, end, start_lat, start_lon, end_lat, end_lon;
        getline(ss,company,',');
        getline(ss,line,',');
        getline(ss,departure,',');
        getline(ss,arrival,',');
        getline(ss,start,',');
        getline(ss,end,',');
        getline(ss,start_lat,',');
        getline(ss,start_lon,',');
        getline(ss,end_lat,',');
        getline(ss,end_lon,',');

        double startLat = std::stod(start_lat);
        double startLong = std::stod(start_lon);
        double endLat = std::stod(end_lat);
        double endLong = std::stod(end_lon);

        time_t departureTime = Utilities::makeTimeFromStr(departure);
        time_t arrivalTime = Utilities::makeTimeFromStr(arrival);
        
        if(not stopDict.count(start))
        {
            stopDict.emplace(start,Stop{.name=start,.latitude=startLat,.longitude=startLong,.neighbours=NeighborMap()});
        }
        if(not stopDict.count(end))
        {
            stopDict.emplace(end,Stop{.name=end,.latitude=endLat,.longitude=endLong,.neighbours=NeighborMap()});
        }
        if(not stopDict.at(start).neighbours.count(end))
        {
            stopDict.at(start).neighbours.emplace(end,std::set<Edge,EdgeComparator>());
        }
        stopDict.at(start).neighbours.at(end).emplace(Edge{.line=company+line,.departureTime=departureTime,.arrivalTime=arrivalTime});
        double timeDiff = Utilities::timeDiff(arrivalTime,departureTime);
        double distance = Utilities::euclidianDistance(startLat,startLong,endLat,endLong);

        if(timeDiff/distance < timeMult && timeDiff/distance != 0)
            timeMult = timeDiff/distance;

    }

    return timeMult;
    
}


double loadData2(const std::string& fileName, StopDict& stopDict, LineStopSets& stopSets, std::vector<std::string>& stops)
{
    double timeMult = std::numeric_limits<double>::infinity();
    std::ifstream file(fileName);
    std::string line;
    getline(file,line);
    while(getline(file,line))
    {
        std::stringstream ss{line};
        getline(ss,line,',');
        std::string company, line, departure, arrival, start, end, start_lat, start_lon, end_lat, end_lon;
        getline(ss,company,',');
        getline(ss,line,',');
        getline(ss,departure,',');
        getline(ss,arrival,',');
        getline(ss,start,',');
        getline(ss,end,',');
        getline(ss,start_lat,',');
        getline(ss,start_lon,',');
        getline(ss,end_lat,',');
        getline(ss,end_lon,',');

        double startLat = std::stod(start_lat);
        double startLong = std::stod(start_lon);
        double endLat = std::stod(end_lat);
        double endLong = std::stod(end_lon);

        time_t departureTime = Utilities::makeTimeFromStr(departure);
        time_t arrivalTime = Utilities::makeTimeFromStr(arrival);
        
        if(not stopDict.count(start))
        {
            stopDict.emplace(start,Stop{.name=start,.latitude=startLat,.longitude=startLong,.neighbours=NeighborMap()});
            stops.push_back(start);
        }
        if(not stopDict.count(end))
        {
            stopDict.emplace(end,Stop{.name=end,.latitude=endLat,.longitude=endLong,.neighbours=NeighborMap()});
            stops.push_back(end);
        }
        if(not stopDict.at(start).neighbours.count(end))
        {
            stopDict.at(start).neighbours.emplace(end,std::set<Edge,EdgeComparator>());
        }
        stopDict.at(start).neighbours.at(end).emplace(Edge{.line=company+line,.departureTime=departureTime,.arrivalTime=arrivalTime});
        double timeDiff = Utilities::timeDiff(arrivalTime,departureTime);
        double distance = Utilities::euclidianDistance(startLat,startLong,endLat,endLong);

        if(timeDiff/distance < timeMult && timeDiff/distance != 0)
            timeMult = timeDiff/distance;

        if(not stopSets.count(company+line))
        {
            stopSets.emplace(company+line, std::unordered_set<std::string>{});
        }
        stopSets.at(company+line).emplace(start);
        stopSets.at(company+line).emplace(end);

    }

    return timeMult;
}