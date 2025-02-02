#pragma once
#include <ctime>
#include <string>

namespace Utilities {
    static constexpr size_t DAY_IN_SECONDS = 86400;

    time_t makeTimeFromStr(const std::string& dateStr);
    int timeDiff(const time_t& t1, const time_t& t2);
    void toStream(std::ostream& os, const time_t& t);
    double euclidianDistance(double startLat, double startLong, double endLat, double endLong);
}
