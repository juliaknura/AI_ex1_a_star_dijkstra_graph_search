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
    LineStopSets stopSets;
    std::vector<std::string> stops;
    int filler;
    if(argc<5)
        return EXIT_FAILURE;
    auto timeMult = loadData2("../../data/connection_graph.csv",d, stopSets, stops);
    
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

    OptimizationType type = OptimizationType::ChangeCount;
    auto multiplier =  CHANGE_MULTIPLIER;

    time_t currTime = Utilities::makeTimeFromStr(currTimeStr);

    auto [recordM, costM] = aStarChangeOptimization(startStop, endStop, currTime, d, multiplier, stopSets, filler);
    std::cerr<<"Cost: "<<costM<<std::endl;
    recordM.erase(recordM.begin());
    for(const auto& elem : recordM)
    {
        std::cout<<elem<<std::endl;
    }

    std::cerr<<"Total time -- miliseconds elapsed: "<<s1.elapsed()<<std::endl;

    return EXIT_SUCCESS;

}