#include "api_v2/tree_transform.hpp"
#include "utils/print_utils.hpp" // DEBUG
#include "utils/utils.hpp"
#include <algorithm>
#include <cmath>
#include <iostream> // DEBUG
#include <numeric>

namespace hypercubes {
namespace slow {
namespace internals {

template <> bool TreeFactory<bool>::make_leaf() { return true; }
template <> bool TreeFactory<bool>::make_node() { return false; }

} // namespace internals

} // namespace slow
} // namespace hypercubes
