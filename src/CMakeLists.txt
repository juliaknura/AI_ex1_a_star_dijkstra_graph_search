add_executable(main
    main.cpp)

target_link_libraries(main
lib)

add_library(lib
    DataLoader.cpp 
    DataStructures.cpp
    Algorithms.cpp 
    Utilities.cpp 
)

add_executable(dijkstra
dijkstra.cpp)

target_link_libraries(
    dijkstra
    lib
)

add_executable(a_star 
a_star.cpp)

target_link_libraries(a_star 
lib)

add_executable(mod_a_star 
mod_a_star.cpp)

target_link_libraries(mod_a_star 
lib)