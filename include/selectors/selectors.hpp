#ifndef SELECTORS_H_
#define SELECTORS_H_
#include "selectors/intervals.hpp"
#include <functional>
#include <string>
#include <vector>

namespace hypercubes {
namespace slow {
namespace selectors_v2 {

struct IndexIntervalMap {

  std::vector<std::string> _strings;
  std::vector<Interval> _intervals;
  IndexIntervalMap(std::vector<std::string> levelnames,
                   std::vector<int> indices);
  Interval operator[](const std::string &key) const;
};

using Selector = std::function<BoolM(const IndexIntervalMap &)>;
using BareSelector = std::function<BoolM(const std::vector<int> &)>;
template <class... Args>
Selector getp(BoolM (*predicate)(const IndexIntervalMap &, Args...),
              Args... args) {
  return [predicate, args...](const IndexIntervalMap &m) {
    return predicate(m, args...);
  };
}
Selector operator&&(Selector, Selector);
Selector operator||(Selector, Selector);
Selector operator!(Selector);

} // namespace selectors_v2
} // namespace slow
} // namespace hypercubes

#endif // SELECTORS_H_
