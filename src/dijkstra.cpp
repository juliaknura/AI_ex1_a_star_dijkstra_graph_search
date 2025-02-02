#include <iostream>
#include "DataStructures.hpp"
#include "Utilities.hpp"
#include "DataLoader.hpp"
#include <ctime>
#include "Algorithms.hpp"
#include <algorithm>
#include <string>
#include "Stopwatch.hpp"
#include "Constants.hpp"
#include <cstdlib> 
#include <time.h> 
#include <fstream>

int main(int argc, char** argv)
{
    stopwatch::Stopwatch s1;

    StopDict d;
    if(argc<5)
        return EXIT_FAILURE;
    auto timeMult = loadData("../../data/connection_graph.csv",d);
    
    std::string startStop = argv[1];
    std::string endStop = argv[2];
    std::string criterion = argv[3];
    std::string currTimeStr = argv[4];

    if(d.find(startStop) == d.end())
    {
        std::cout<<startStop<<" is an invalid stop name"<<std::endl;
        return EXIT_FAILURE;
    }
    if(d.find(endStop) == d.end())
    {
        std::cout<<endStop<<" is an invalid stop name"<<std::endl;
        return EXIT_FAILURE;
    }

    OptimizationType type = criterion == "t" ? OptimizationType::Time : OptimizationType::ChangeCount;

    time_t currTime = Utilities::makeTimeFromStr(currTimeStr);

    auto [recordD, costD] = dijkstra(startStop, endStop, currTime, d, type);
    std::cerr<<"Cost: "<<costD<<std::endl;
    recordD.erase(recordD.begin());
    for(const auto& elem : recordD)
    {
        std::cout<<elem<<std::endl;
    }

    std::cerr<<"Total time -- miliseconds elapsed: "<<s1.elapsed()<<std::endl;

    return EXIT_SUCCESS;
}