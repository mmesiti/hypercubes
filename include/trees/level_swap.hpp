#ifndef LEVEL_SWAP_H
#define LEVEL_SWAP_H

#include "partitioners/partitioners.hpp"
#include "utils/utils.hpp"
#include <algorithm>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <vector>
namespace std {
std::string to_string(std::string s);
}
namespace hypercubes {
namespace slow {
namespace internals {

/* moves each elements of 'in' in the corresponding position
 * given by 'permutation'. */
template <class T>
vector<T> apply_permutation(const vector<int> &permutation,
                            const vector<T> &in) {
  vector<T> out(in.size());
  for (int i = 0; i < in.size(); ++i)
    out[permutation[i]] = in[i];
  return {out};
}

template <class T>
bool check_is_permutation(const vector<T> &oldnames, //
                          const vector<T> &newnames) {
  return oldnames.size() == newnames.size() and
             [&oldnames, &newnames]() -> bool {
    return std::all_of(oldnames.begin(), oldnames.end(),
                       [&newnames](const std::string &n) {
                         return std::find(newnames.begin(), newnames.end(),
                                          n) != newnames.end();
                       });
  }();
}

template <class T>
bool check_levels_exist(const vector<T> &refnames, //
                        const vector<T> &oldnames) {

  for (auto levelname : refnames) {
    if (std::find(oldnames.begin(), oldnames.end(), levelname) ==
        oldnames.end())
      return false;
  }
  return true;
}

/* Finds, for every element in 'in', where it is located in 'out'. */
template <class T>
std::vector<int> find_permutation(const std::vector<T> &in,
                                  const std::vector<T> &out) {
  if (in.size() != out.size())
    throw std::invalid_argument(
        std::string("Error, in and out have different lengths."));

  return vtransform(in, [&out, &in](const T &in_el) { //
    auto found = std::find(out.begin(),               //
                           out.end(),                 //
                           in_el);
    if (found == out.end()) {
      std::stringstream ss;
      ss << "Error, " << in_el << "not found in out vector.\n";
      ss << "in vector: " << in << std::endl;
      ss << "out vector: " << out << std::endl;
      throw std::invalid_argument(ss.str());
    }
    return int(found - out.begin());

  });
}

template <class T>
std::vector<int> find_permutation(const std::vector<T> &original, //
                                  const std::vector<T> &in,       //
                                  const std::vector<T> &out) {

  if (in.size() != out.size())
    throw std::invalid_argument(
        std::string("Error, in and out have different lengths."));

  // check that all elements in 'in' and 'out'
  // are present in 'original'
  for (auto &i : in)
    if (std::find(original.begin(), original.end(), i) == original.end())
      throw std::invalid_argument(std::string("Element ") + std::to_string(i) +
                                  " from 'in' not found in original vector.");
  for (auto &i : out)
    if (std::find(original.begin(), original.end(), i) == original.end())
      throw std::invalid_argument(std::string("Element ") + std::to_string(i) +
                                  " from 'out' not found in original vector.");

  decltype(in) fullout = vtransform(original, [&in, &out](T o) {
    for (int i = 0; i < in.size(); ++i)
      if (in[i] == o)
        return out[i];
    return o;
  });
  return find_permutation(original, fullout);
}

std::vector<std::string> get_partitioners_names(const PartList &partitioners);
} // namespace internals
} // namespace slow
} // namespace hypercubes
#endif
