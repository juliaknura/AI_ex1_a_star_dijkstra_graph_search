#include "Utilities.hpp"
#include <iostream>
#include <cmath>

namespace Utilities {
time_t makeTimeFromStr(const std::string& dateStr)
{
    auto hour{std::stoi(dateStr.substr(0,2))}, minute{std::stoi(dateStr.substr(3,2))}, second{std::stoi(dateStr.substr(6,2))};
    if(hour > 23)
        hour-=24;

    tm temp{.tm_sec=second,.tm_min=minute,.tm_hour=hour};
    return mktime(&temp);
}

int timeDiff(const time_t& t1, const time_t& t2)
{
    int diff = difftime(t1,t2);
    if(diff<0)
    {
        time_t t1_2 = t1 + DAY_IN_SECONDS;
        return difftime(t1_2,t2);
    }
    return diff;
}

void toStream(std::ostream& os, const time_t& t)
{
    os<<std::string_view(ctime(&t)).substr(11,8);
}

double euclidianDistance(double startLat, double startLong, double endLat, double endLong)
{
    return std::sqrt(std::pow(startLat-endLat,2) + std::pow(startLong-endLong,2));
}
}