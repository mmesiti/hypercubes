#include "selectors/intervals.hpp"
#include "selectors/bool_maybe.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace hypercubes {
namespace slow {
const int Interval::LIMIT = 1000;
Interval::Interval(int f, int s) : min(f), max(s) {}
bool Interval::contains(int x) const { return min <= x and x < max; }
bool Interval::empty() const {
  return not contains(min); // and not contains(max),
                            // but that's trivial
                            // given how "contains" works.
}

bool Intervals::contain(int x) const {
  return std::any_of(vs.begin(), vs.end(),
                     [x](auto v) { return v.contains(x); });
}
bool same(Interval ab, Interval cd) {
  return ab.min == cd.min and ab.max == cd.max;
}

Interval operator&&(Interval ab, Interval cd) {
  return {std::max(ab.min, cd.min), std::min(ab.max, cd.max)};
}
Interval operator-(Interval ab) { return {-ab.max + 1, -ab.min + 1}; }
Interval operator+(const Interval &ab, const Interval &cd) {
  return {ab.min + cd.min, (ab.max - 1) + (cd.max - 1) + 1};
}
Interval operator-(const Interval &ab, const Interval &cd) {
  return ab + (-cd);
}
Interval operator*(const Interval &ab, const Interval &cd) {
  int a = ab.min * cd.min;
  int b = ab.min * (cd.max - 1);
  int c = (ab.max - 1) * cd.min;
  int d = (ab.max - 1) * (cd.max - 1);
  return {std::min({a, b, c, d}), std::max({a, b, c, d}) + 1};
}
Interval operator/(const Interval &ab, int c) {
  int a = ab.min / c;
  int b = (ab.max - 1) / c;
  return {std::min(a, b), std::max(a, b) + 1};
}

BoolM operator<(const Interval &ab, const Interval &cd) {
  if (ab.empty() or cd.empty())
    return BoolM::M;

  if (ab.min < cd.min) {
    if (ab.max - 1 < cd.min)
      return BoolM::T;
    else
      return BoolM::M;
  } else if (ab.min == cd.min) {
    if (cd.max == ab.min + 1)
      return BoolM::F;
    else
      return BoolM::M;
  } else { // ab.min > cd.min
    if (cd.max <= ab.min + 1)
      return BoolM::F;
    else
      return BoolM::M;
  }
}
BoolM operator<(const Interval &ab, int c) {
  if (ab.empty())
    return BoolM::F;
  Interval cd(c, c + 1);
  return ab < cd;
}
BoolM operator<(int c, const Interval &ab) {
  if (ab.empty())
    return BoolM::F;
  Interval cd(c, c + 1);
  return cd < ab;
}
BoolM operator==(const Interval &ab, const Interval &cd) {
  if (ab.empty() or cd.empty())
    return BoolM::M;
  if (ab.max == ab.min + 1 and cd.max == cd.min + 1) {
    if (ab.min == cd.min)
      return BoolM::T;
    else
      return BoolM::F;
  } else {
    if ((ab and cd).empty())
      return BoolM::F;
    else
      return BoolM::M;
  }
}

// TODO
BoolM operator<=(const Interval &ab, const Interval &cd) {
  if (ab.empty() or cd.empty())
    return BoolM::M;

  if (ab.min < cd.min) {
    if (ab.max + 1 <= cd.min)
      return BoolM::T;
    else
      return BoolM::M;
  } else if (ab.min == cd.min) {
    return BoolM::M;
  } else { // ab.min > cd.min
    if (cd.max < ab.min + 1)
      return BoolM::F;
    else
      return BoolM::M;
  }
}
// Multiple interval operations

Intervals::Intervals(std::vector<Interval> _vs) : vs(_vs) {
  bool first = true;
  int last_max;
  for (auto &ab : _vs) {
    if (not(ab.min < ab.max))
      throw std::invalid_argument("Empty interval in initialisation");
    if (first) {
      first = false;
    } else if (not(last_max < ab.min))
      throw std::invalid_argument("Overlapping intervals");
    last_max = ab.max;
  }
};
Intervals::Intervals() : vs(){};

bool same(Intervals abs, Intervals cds) {
  if (abs.vs.size() != cds.vs.size())
    return false;
  for (int i = 0; i < abs.vs.size(); ++i)
    if (not same(abs.vs[i], cds.vs[i]))
      return false;
  return true;
}
Intervals operator!(Interval v) {
  int low_max = v.min - 1;
  int high_min = v.max + 1;
  Interval low(-Interval::LIMIT, low_max);
  Interval high(high_min, Interval::LIMIT);
  std::vector<Interval> is;
  if (not low.empty())
    is.push_back(low);
  if (not high.empty())
    is.push_back(high);
  return is;
}
Intervals operator||(Interval ab, Interval cd) {
  if ((ab and cd).empty()) {
    if (ab.min < cd.min)
      return std::vector<Interval>{ab, cd};
    else
      return std::vector<Interval>{cd, ab};
  } else {
    return std::vector<Interval>{
        {std::min(ab.min, cd.min), std::max(ab.max, cd.max)}};
  }
}

Intervals operator!(Intervals abs) {
  std::vector<int> fences;
  fences.push_back(-Interval::LIMIT);
  for (auto ab : abs.vs) {
    fences.push_back(ab.min);
    fences.push_back(ab.max);
  }
  fences.push_back(Interval::LIMIT);
  std::vector<Interval> vs;
  for (int i = 0; i < fences.size(); i += 2) {
    int start = fences[i];
    int end = fences[i + 1];

    if (end > start)
      vs.push_back({start, end});
  }

  return vs;
}
static Intervals check_range(Intervals abs, Intervals cds,
                             bool (*op)(bool, bool)) {
  std::vector<int> poles;
  for (auto c : {abs, cds})
    for (auto ab : c.vs) {
      poles.push_back(ab.min);
      poles.push_back(ab.max);
    }
  std::sort(poles.begin(), poles.end());
  std::vector<Interval> vs;
  int start;
  bool was_in_interval = false;
  for (auto &f : poles) {
    bool now_in_interval = op(abs.contain(f), cds.contain(f));
    bool entering = not was_in_interval and now_in_interval;
    bool exiting = was_in_interval and not now_in_interval;
    if (entering)
      start = f;
    if (exiting) // exiting interval
    {
      vs.push_back({start, f});
    }
    was_in_interval = now_in_interval;
  }
  return vs;
}
Intervals operator&&(Intervals abs, Intervals cds) {
  return check_range(abs, cds, [](bool a, bool b) -> bool { return a and b; });
}
Intervals operator||(Intervals abs, Intervals cds) {
  return check_range(abs, cds, [](bool a, bool b) -> bool { return a or b; });
}
Intervals operator-(Intervals ab) {
  std::vector<Interval> vs;
  // order of intervals need to be reversed!
  std::transform(ab.vs.rbegin(), ab.vs.rend(), std::back_inserter(vs),
                 [](Interval a) { return -a; });
  return vs;
}
} // namespace slow
} // namespace hypercubes
