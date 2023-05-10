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
  _strings = levelnames;
  for (int i = 0; i < indices.size(); ++i)
    _intervals.push_back(Interval(indices[i], indices[i] + 1));
  // TODO: better-informed intervals if needed  (e.g., HBB only goes up to 4)
  for (int i = indices.size(); i < levelnames.size(); ++i) {
    _intervals.push_back(Interval(0, // All indices start from 0 anyway.
                                  Interval::LIMIT));
  }
}

Interval IndexIntervalMap::operator[](const std::string &key) const {
  auto it = std::find(_strings.begin(), _strings.end(), key);
  if (it == _strings.end())
    throw std::invalid_argument("Key not found in IndexIntervalMap.");

  return _intervals[it - _strings.begin()];
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
