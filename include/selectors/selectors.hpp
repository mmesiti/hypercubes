#ifndef SELECTORS_H_
#define SELECTORS_H_
#include "selectors/intervals.hpp"
#include <string>
#include <vector>

namespace hypercubes {
namespace slow {
namespace selectors_v2 {

class IndexIntervalMap {
  std::vector<std::pair<std::string, Interval>> _map;

public:
  IndexIntervalMap(std::vector<std::string> levelnames,
                   std::vector<int> indices);
  Interval operator[](std::string key) const;
};

} // namespace selectors_v2
} // namespace slow
} // namespace hypercubes

#endif // SELECTORS_H_
