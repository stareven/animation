#pragma once

#include <sys/time.h>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <limits>

namespace animation {

typedef int64_t TTime;
static const TTime TTimeMax = std::numeric_limits<TTime>::max();

class Utils
{
public:
    static TTime now()
    {
        timeval tv;
        gettimeofday(&tv, nullptr);
        return tv.tv_sec * 1e3 + tv.tv_usec * 1e-3;
    }

};


}
