#ifndef LEVEL_NAMES_H_
#define LEVEL_NAMES_H_
#include <ostream>
#include <vector>
namespace hypercubes {
namespace slow {
namespace internals {

struct LevelName {
  int name;
  bool operator==(const LevelName &other) const { return name == other.name; }
};
std::vector<LevelName>
/** Generates a new list of levels,
 * naming the new level that has been inserted
 * with a new integer not used before
 * (greater than any of the currently used levels)
 * The new level is inserted at the position
 * indicated by 'level'
 *  */
insert_new_level_name(const std::vector<LevelName> &levels, int level);

/** Generates a new list of levels,
 * removing the levels in the range [levelstart,levelend)
 * and naming the new resulting level
 * with a new integer not used before
 * (greater than any of the currently used levels)
 *  */
std::vector<LevelName> flatten_levels(const std::vector<LevelName> &levels,
                                      int levelstart, int levelend);

int find_level(LevelName l, const std::vector<LevelName> &levels);
} // namespace internals
} // namespace slow
} // namespace hypercubes
namespace std {
std::ostream &operator<<(std::ostream &os,
                         const hypercubes::slow::internals::LevelName &l);

} // namespace std

#endif // LEVEL_NAMES_H_
