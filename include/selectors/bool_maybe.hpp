#ifndef BOOL_MAYBE_H_
#define BOOL_MAYBE_H_
#include <ostream>

namespace hypercubes {
namespace slow {

enum class BoolM { F = 0, M = 1, T = 2 };
int to_int(BoolM);
bool to_bool(BoolM);
BoolM operator&&(BoolM, BoolM);
BoolM operator||(BoolM, BoolM);
BoolM operator!(BoolM);
} // namespace slow
} // namespace hypercubes

namespace std {
std::ostream &operator<<(std::ostream &os, hypercubes::slow::BoolM x);

}

#endif // BOOL_MAYBE_H_
