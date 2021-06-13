#ifndef EXCEPTIONS_H_
#define EXCEPTIONS_H_
#include <stdexcept>
namespace hypercubes {
namespace slow {
namespace internals {
class KeyNotFoundError : public std::invalid_argument {
public:
  template <class... Args>
  KeyNotFoundError(Args... args) : std::invalid_argument(args...){};
};
class TreeLevelPermutationError : public std::invalid_argument {
public:
  template <class... Args>
  TreeLevelPermutationError(Args... args) : std::invalid_argument(args...){};
};
} // namespace internals
} // namespace slow
} // namespace hypercubes

#endif // EXCEPTIONS_H_
