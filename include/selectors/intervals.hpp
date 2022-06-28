#ifndef INTERVAL_H_
#define INTERVAL_H_
#include "bool_maybe.hpp"
#include <vector>

namespace hypercubes {
namespace slow {

/* An integer value represented by a single <min,max> range */
struct Interval {
  // We should use INT_MIN and INT_MAX,
  // but this would make our life much more complex.
  // This is thought to work with small integers anyway.
  static const int LIMIT;
  int min, max; // [min,max)
  Interval(int a, int b);
  bool empty();
  bool contains(int x) const;
};

/* An integer value represented as a list of intervals of integers. */
struct Intervals {
  // A "list" of pair [low,high) sorted by min.
  // Intervals are disjoint.
  std::vector<Interval> vs;
  Intervals(std::vector<Interval> _vs);
  Intervals();
  bool contain(int x) const;
};

bool same(Interval, Interval);   // not to be confused with ==
bool same(Intervals, Intervals); // not to be confused with ==

Intervals operator!(Interval);
Interval operator&&(Interval, Interval);
Intervals operator||(Interval, Interval);

Intervals operator!(Intervals);
Intervals operator&&(Intervals, Intervals);
Intervals operator||(Intervals, Intervals);

Interval operator-(Interval);
Intervals operator-(Intervals);
Interval operator+(const Interval &, const Interval &);
Interval operator-(const Interval &, const Interval &);
Interval operator*(const Interval &, const Interval &);
Interval operator/(const Interval &, int);
// Interval operator/(Interval, Interval) is too messy and possibly not useful.
// TODO
Intervals operator+(const Intervals &, const Intervals &);
Intervals operator-(const Intervals &, const Intervals &);
Intervals operator*(const Intervals &, const Intervals &);
Intervals operator/(const Intervals &, int);

BoolM operator<(const Interval &, const Interval &);
BoolM operator<=(const Interval &, const Interval &);
BoolM operator>(const Interval &, const Interval &);
BoolM operator>=(const Interval &, const Interval &);
BoolM operator==(const Interval &, const Interval &);
BoolM operator!=(const Interval &, const Interval &);

} // namespace slow
} // namespace hypercubes
#endif // INTERVAL_H_
