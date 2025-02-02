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
    stopwatch::Stopwatch s1, s2, s3, s4;

    StopDict d;
    LineStopSets stopSets;
    std::vector<std::string> stops;
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

    OptimizationType type = criterion == "t" ? OptimizationType::Time : OptimizationType::ChangeCount;
    auto multiplier = type == OptimizationType::Time? timeMult : CHANGE_MULTIPLIER;
    // std::cout<<multiplier<<std::endl;

    time_t currTime = Utilities::makeTimeFromStr(currTimeStr);

    ////////////////////////////////////////////////////

    std::srand(time(NULL));
    std::vector<double> computationTimesAStar;
    std::vector<double> computationTimesAStarM;
    std::vector<double> computationTimesDijkstra;
    std::vector<double> costsAStar;
    std::vector<double> costsAStarM;
    std::vector<double> costsDijkstra;
    std::vector<int> visitedStopsVec;
    std::vector<int> visitedStopsVecM;
    computationTimesAStar.reserve(1000);
    computationTimesAStarM.reserve(1000);
    computationTimesDijkstra.reserve(1000);
    costsAStar.reserve(1000);
    costsAStarM.reserve(1000);
    costsDijkstra.reserve(1000);
    visitedStopsVec.reserve(1000);
    visitedStopsVecM.reserve(1000);
    std::cout<<"before loop"<<std::endl;
    
    for(int i=0;i<1000;i++)
    {
        std::cout<<i<<"\t";
        std::cout.flush();
        auto index = rand()%stops.size();
        const auto& start = stops.at(index);
        index = rand()%stops.size();
        const auto& end = stops.at(index);
        int visitedStops{};
        s1.start();
        auto [record, cost] = aStar(start, end, currTime, d, type, multiplier, visitedStops);
        computationTimesAStar.push_back(s1.elapsed());
        costsAStar.push_back(cost);
        visitedStopsVec.push_back(visitedStops);
        s1.start();
        auto [record2, cost2] = dijkstra(start, end, currTime, d, type);
        computationTimesDijkstra.push_back(s1.elapsed());
        costsDijkstra.push_back(cost2);
        visitedStops = 0;
        s1.start();
        auto [record3, cost3] = aStarChangeOptimization(start, end, currTime, d, multiplier, stopSets, visitedStops);
        computationTimesAStarM.push_back(s1.elapsed());
        costsAStarM.push_back(cost3);
        visitedStopsVecM.push_back(visitedStops);
    }

    std::ofstream file("output.csv");
    file<<"timeMAstar,costMAstar,stopsMAstar,timeAstar,costAstar,stopsAStar,timeDijkstra,costDijkstra\n";
    for(int i=0;i<1000;i++)
    {
        file<<computationTimesAStarM.at(i)<<","<<costsAStarM.at(i)<<","<<visitedStopsVecM.at(i)<<","<<computationTimesAStar.at(i)<<","<<costsAStar.at(i)<<","<<visitedStopsVec.at(i)<<","<<computationTimesDijkstra.at(i)<<","<<costsDijkstra.at(i)<<"\n";
    }

    // std::cout<<"A STAR"<<std::endl;
    // s2.start();
    // auto [record, cost] = aStar(startStop, endStop, currTime, d, type, multiplier);
    // std::cout<<"Computations time -- miliseconds elapsed: "<<s2.elapsed()<<std::endl;
    // std::cout<<"Cost: "<<cost<<std::endl;
    // record.erase(record.begin());
    // for(const auto& elem : record)
    // {
    //     std::cout<<elem<<std::endl;
    // }

    // std::cout<<"DIJKSTRA"<<std::endl;
    // s3.start();
    // auto [recordD, costD] = dijkstra(startStop, endStop, currTime, d, type);
    // std::cout<<"Computations time -- miliseconds elapsed: "<<s3.elapsed()<<std::endl;
    // std::cout<<"Cost: "<<costD<<std::endl;
    // recordD.erase(recordD.begin());
    // for(const auto& elem : recordD)
    // {
    //     std::cout<<elem<<std::endl;
    // }

    // if(type == OptimizationType::ChangeCount)
    // {
    //     std::cout<<"MODIFIED A STAR"<<std::endl;
    //     s4.start();
    //     auto [recordM, costM] = aStarChangeOptimization(startStop, endStop, currTime, d, multiplier, stopSets);
    //     std::cout<<"Computations time -- miliseconds elapsed: "<<s4.elapsed()<<std::endl;
    //     std::cout<<"Cost: "<<costM<<std::endl;
    //     recordM.erase(recordM.begin());
    //     for(const auto& elem : recordM)
    //     {
    //         std::cout<<elem<<std::endl;
    //     }
    // }

    // std::cout<<"Total time -- miliseconds elapsed: "<<s1.elapsed()<<std::endl;

    return EXIT_SUCCESS;
}