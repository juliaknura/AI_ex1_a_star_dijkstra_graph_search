# Route finding in Wroclaw public transport - graph search with Dijkstra and A* algorithms

A program for finding the most optimal route between two public transport stops in terms of two criteria: the shortest travel time and the least number of transfers. Two algorithms were implemented to perform this task: Dijkstra's algorithm for finding the shortest paths in a weighted graph, and the heuristic A* algorithm which is an extension of the Dijkstra algorithm. 

## Representing public transport data as a graph

Input data [Open data Wroclaw](https://opendata.cui.wroclaw.pl/dataset/rozkladjazdytransportupublicznegoplik_data) of the program in form of a `.csv` file was a list of connections in the form of rows containing the following data:

1. `id` - connection id
1. `company` - name of the public transport company
1. `line` - id of the line
1. `departure_time` (HH:MM:SS format)
1. `arrival_time` (HH:MM:SS format)
1. `start_stop`
1. `end_stop`
1. `start_stop_lat` - latitude of the starting stop in WGS84  
1. `start_stop_lon` - longitude of the starting stop in WGS84
1. `end_stop_lat` - latitude of the ending stop in WGS84
1. `end_stop_lon` - longitude of the ending stop in WGS84

The stops uniquely identified by their names were taken as the nodes of the graph. A certain simplification has been applied here - in reality, there are usually at least two stops with the same name, representing opposing directions (in rare cases, as in case of Nadodrze Railway Station stop, more), each with its own differenct geographical location. However, it was assumed that stops with the same name are located close enough to each other to treat them as a single entity, and the traveler is able to move between them in zero time. Since in the dataset exist stops from which travel is only possible in one direction and the routes of some lines can differ in opposite directions (for example, line 7 direction Klecina goes along the route `Dubois -> Wroclaw University -> Market Square -> National Forum of Music -> Renoma`, while line 7 direction Poświętne goes along the route `Renoma -> Opera -> Świdnicka -> Oławska -> Wita Stwosza -> Ossolineum -> Dubois`), a directed graph was used.

The stop is represented by the following data structure (`DataStructures.hpp`): 

```
struct Stop{
    std::string name;
    double latitude;
    double longitude;
    NeighborMap neighbours;
};
```

Where `neighbours` is a dictionary of neighbouring stops:

```
using NeighborMap = std::unordered_map<std::string, std::set<Edge,EdgeComparator>>;
```

The dictionary assigns to each neighbour a list of edges that connect the stop to that neighbour, ordered by their departure time. The edge is represented by the following structure:

```
struct Edge {
    std::string line;
    time_t departureTime;
    time_t arrivalTime;
};
```

Each edge contains info about the line (created as combination of line id and company name), departure time and arrival to neighbouring stop time.

All stops were stored in a dictionary where the key was the name of the stop. 

```
using StopDict = std::unordered_map<std::string, Stop>;
```

To represent a route found by the algorithms, the following structure was used:

```
struct RouteRecord { 
    std::string line; 
    time_t departureTime; 
    time_t arrivalTime; 
    std::string startName; 
    std::string stopName; 
}; 
```

## Calculation of the cost between two nodes (best edge selection) 

Another problem encountered was how to calculate the cost of a given edge. Since there are many edges between two nodes, calculating the weight of a given connection consisted of choosing the best edge according to a chosen optimization criterion and fastest departure time from the starting point of the journey.

When the time optimization criterion is taken into account, the first and most obvious idea would be to choose the first edge that connects the two nodes after arriving at the starting stop. This is the most likely optimal choice. However, there is a risk that any of the next edges (ranked according to departure time) would have a later time of departure time, but the travel time would be so short that the arrival time would be earlier from the arrival time of the first ranked edge (i.e. there would be an “overtaking” of the of the earlier edge by the later edge). Therefore, in addition to the edge with the earliest departure time, all those are also taken into account whose departure time departure time is earlier than the arrival time of the first analyzed edge (later even zero travel time would not make such an edge better). From this set of edges is selected the one whose cost (= arrival time - time of appearance at the initial stop) is the smallest.

When the transfer optimization criterion is taken into account, the selected edge is the one with the fastest departure time (after appearing at the starting stop) that does not require a change of line. If this is not possible (between the two analyzed stops does not run the line by which the trip was made so far), the edge with the fastest departure time is selected.

## Chosen heuristics

An integral part of the implementation of the A* algorithm is the selection of a heuristic (a function that estimates the cost from a given node to the final node) suitable for a given case. To ensure that the A* algorithm works correctly, the heuristics should be optimistic (acceptable) - the cost function it estimates should not exceed the value of the actual cost function between the two analyzed nodes. The heuristics function was implemented as the Manhattan distance between two nodes multiplied by a multiplier appropriate for a given optimization criterion. Finding the appropriate heuristics was reduced to the problem of determining the value of the multiplier.

For the shortest time criterion, the speed of communication vehicles was analized at all edges and, taking into account the fact that the heuristics should not overestimate the cost, the fastest value was selected.

The multiplier was defined as inverse of speed, so that the values of the heuristics function were as small as possible.

To determine the value of the multiplier for the transfer criterion, the Dijkstra algorithm was used where the number of transfers was taken as the cost of the route. For random routes, the optimal number of transfers was determined, and then the multiplier was determined as the minimum from the set of values (number of transfers)/(route length).

## Comparison of Dijkstra and A-star

The Stopwatch class from the [CrikeeIP Stopwatch](https://github.com/CrikeeIP/Stopwatch) repository, available under a free license, was used to compare the results of the algorithms. The code is included in the Stopwatch.hpp file. 

A* and Dijkstra algorithms (for both optimization criteria) were compared in terms of three criteria - computation time, number of visited graph nodes before finding the final solution and the presented solution itself. 

For shortest time optimization, A* performed significantly better compared to the Dijkstra algorithm. By adopting the right heuristics, A* never gave worse results than Dijkstra (both were always optimal).

On average, A* visited 468 fewer graph nodes before finding a solution, and the computation took 111 milliseconds less on average. It always had an advantage over Dijkstra - for a thousand randomly selected routes, the smallest gain was 41 milliseconds of computation time and three fewer nodes visited. 

For the transfer optimization, the results were not similarly clear. On average, A* would find a route with 0.004 more transfers. This was most likely due to non-ideal heuristics - the development of heuristics could be proposed as a further line of work. The gain in time and in the number of nodes searched was similar here for A* as with the shortest time criterion.

## A-star algorithm modification

During testing, it was noticed that the algorithm with the transfer optimization criterion did not always give objectively optimal results - sometimes, despite the existence of a direct connection between the start and end stops, the algorithm recommended at least one more transfer. After analysis, it turned out that this was due to edge selection method. 
The moment of boarding from the start stop to the first vehicle is treated as a transfer (after all, the previous “no” line is different from any line that can be departed from the stop), so that the function always returned the “first better” edge, which in practice meant choosing the vehicle that departed the stop the fastest from the start of the trip. 
If there was a direct line between the start and end stops and the first vehicle that was selected was that line, the solution found was optimal. However, when a vehicle of a different, non-direct line was selected first, this resulted in the need for at least one more transfer. A modification was therefore introduced that allows the existence of direct connections to be examined when the first edge of travel is selected. A new data structure was used for this, a dictionary that stored a list of all the stops that a line visits.

```
using LineStopSets = std::unordered_map<std::string, std::unordered_set<std::string>>;
```

When the algorithm was faced with choosing the first edge in the entire journey, it checked all lines running from the starting stop and a subset of all direct ones was selected. Then, only from this subset the earliest connection was selected. This modification made it possible to improve results of the algorithm in most cases where the starting and ending stops were connected by a direct line.
