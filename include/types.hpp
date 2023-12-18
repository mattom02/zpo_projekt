#ifndef NETSIM_TYPES_HPP
#define NETSIM_TYPES_HPP

#include <functional>

using ElementID = unsigned long long;

using Time = unsigned long long;

using TimeOffset = unsigned long long;

using ProbabilityGenerator = std::function<double()>;

#endif 

