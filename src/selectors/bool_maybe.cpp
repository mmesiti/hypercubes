#include "selectors/bool_maybe.hpp"

namespace hypercubes {
namespace slow {
int to_int(BoolM x) {
  switch (x) {
  case BoolM::F:
    return 0;
  case BoolM::M:
    return 1;
  case BoolM::T:
    return 2;
  }
}
BoolM operator&&(BoolM x, BoolM y) {
  const BoolM and_truth_table[3][3]{{BoolM::F, BoolM::F, BoolM::F}, //
                                    {BoolM::F, BoolM::M, BoolM::M}, //
                                    {BoolM::F, BoolM::M, BoolM::T}};
  return and_truth_table[to_int(x)][to_int(y)];
}
BoolM operator||(BoolM x, BoolM y) {
  const BoolM or_truth_table[3][3]{{BoolM::F, BoolM::M, BoolM::T}, //
                                   {BoolM::M, BoolM::M, BoolM::T}, //
                                   {BoolM::T, BoolM::T, BoolM::T}};
  return or_truth_table[to_int(x)][to_int(y)];
}
BoolM operator!(BoolM x) {
  const BoolM not_truth_table[3]{BoolM::T, BoolM::M, BoolM::F};
  return not_truth_table[to_int(x)];
}
bool to_bool(BoolM x) {
  switch (x) {
  case BoolM::F:
    return false;
  default:
    return true;
  }
}
} // namespace slow
} // namespace hypercubes

namespace std {
using namespace hypercubes::slow;
std::ostream &operator<<(std::ostream &os, BoolM x) {
  switch (x) {
  case BoolM::F:
    os << 'F';
    break;
  case BoolM::M:
    os << 'M';
    break;
  case BoolM::T:
    os << 'T';
    break;
  }
  return os;
}

} // namespace std
