#include "api_v2/level_names.hpp"
#include <algorithm>

namespace hypercubes {
namespace slow {
namespace internals {
using std::vector;
vector<LevelName> insert_new_level_name(const vector<LevelName> &levels,
                                        int level) {
  vector<LevelName> out;
  out.reserve(levels.size() + 1);
  LevelName new_name{1 + std::max_element(levels.begin(), //
                                          levels.end(),   //
                                          [](LevelName a, LevelName b) {
                                            return a.name < b.name;
                                          })
                             ->name};

  for (int i = 0; i < levels.size(); ++i) {
    if (i == level)
      out.push_back(new_name);
    out.push_back(levels[i]);
  }

  return out;
}
vector<LevelName> flatten_levels(const vector<LevelName> &levels,
                                 int levelstart, int levelend) {
  vector<LevelName> out;
  out.reserve(levels.size() + 1 - (levelend - levelstart));
  LevelName new_name{1 + std::max_element(levels.begin(), //
                                          levels.end(),   //
                                          [](LevelName a, LevelName b) {
                                            return a.name < b.name;
                                          })
                             ->name};
  for (int i = 0; i < levels.size(); ++i) {
    if (i == levelstart)
      out.push_back(new_name);
    if (levelstart <= i and i < levelend)
      continue;
    out.push_back(levels[i]);
  }

  return out;
}

int find_level(const LevelName l, const std::vector<LevelName> &levels) {
  for (int i = 0; i < levels.size(); ++i)
    if (levels[i] == l)
      return i;
  return -1;
}
} // namespace internals
} // namespace slow
} // namespace hypercubes
namespace std {
std::ostream &operator<<(std::ostream &os,
                         const hypercubes::slow::internals::LevelName &l) {
  os << l.name;
  return os;
}

} // namespace std
