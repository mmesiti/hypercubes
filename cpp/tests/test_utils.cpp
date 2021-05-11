#include "test_utils.hpp"

namespace std {

std::ostream &operator<<(std::ostream &os, std::vector<int> const &v) {
  os << "{";
  for (const auto &x : v) {
    os << x << ", ";
  }
  os << "}\n";

  return os;
};

} // namespace std
namespace data {
/**
 *  A random list factory that yields edge cases first, then random
 *  To be used as a data generator with boost/test.
 */
std::vector<int> rilist::iterator::operator*() const { return current_vector; }

void rilist::iterator::operator++() {
  if (currentsize <= maxsize) {
    int max_idx = std::pow(2, currentsize);
    in_size_idx++;
    if (in_size_idx >= max_idx) {
      currentsize++;
      in_size_idx = 0;
    }
  }
  if (currentsize > maxsize)
    current_vector = this->random_vector();
  else
    current_vector = this->sequence_vector();
}
rilist::iterator::iterator(int min_, int max_, int minsize_, int maxsize_)
    : min(min_), max(max_), minsize(minsize_), maxsize(maxsize_),
      currentsize(minsize_), in_size_idx(0) {
  current_vector = this->sequence_vector();
};

std::vector<int> rilist::iterator::random_vector() const {
  std::vector<int> res;
  int random_size = (minsize + rand()) % (maxsize - minsize + 1) + minsize;
  for (int s = 0; s < random_size; s++) {
    int random_value = (min + rand()) % (max - min + 1) + min;
    res.push_back(random_value);
  }
  return res;
}

std::vector<int> rilist::iterator::sequence_vector() const {
  std::vector<int> res;
  int idx = in_size_idx;
  for (int s = 0; s < currentsize; s++) {
    res.push_back(idx % 2 ? min : max);
    idx /= 2;
  }
  return res;
}

rilist::iterator rilist::begin() const {
  return iterator(min, max, minsize, maxsize);
}
boost::unit_test::data::size_t rilist::size() const {
  return boost::unit_test::data::BOOST_TEST_DS_INFINITE_SIZE;
}

} // namespace data
