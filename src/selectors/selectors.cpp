#include "selectors/selectors.hpp"
#include "selectors/intervals.hpp"
#include <algorithm>

namespace hypercubes {
namespace slow {
namespace selectors_v2 {

IndexIntervalMap::IndexIntervalMap(std::vector<std::string> levelnames,
                                   std::vector<int> indices) {
  if (indices.size() > levelnames.size())
    throw std::invalid_argument(
        "indices vector must not be longer than levelnames vector.");
  for (int i = 0; i < indices.size(); ++i) {
    _map.push_back(std::make_pair(levelnames[i], //
                                  Interval(indices[i], indices[i] + 1)));
  }
  // TODO: better-informed intervals if needed.
  for (int i = indices.size(); i < levelnames.size(); ++i) {
    _map.push_back(
        std::make_pair(levelnames[i], //
                       Interval(0,    // All indices start from 0 anyway.
                                Interval::LIMIT)));
  }
}

Interval IndexIntervalMap::operator[](const std::string &key) const {
  auto it = std::find_if(_map.begin(), _map.end(),
                         [key](auto p) { return p.first == key; });
  if (it == _map.end())
    throw std::invalid_argument("Key not found in IndexIntervalMap.");

  return it->second;
};

Selector operator&&(Selector a, Selector b) {
  return [a, b](const IndexIntervalMap &map) { return a(map) and b(map); };
}
Selector operator||(Selector a, Selector b) {
      return [a, b](const IndexIntervalMap &map) { return a(map) or b(map); };
}
Selector operator!(Selector s) {
    return [s](const IndexIntervalMap &map) { return not s(map); };
}

} // namespace selectors_v2
} // namespace slow
} // namespace hypercubes
