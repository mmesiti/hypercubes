#ifndef UTILS_H_
#define UTILS_H_
#include <boost/test/data/monomorphic.hpp>
#include <ostream>
#include <vector>

namespace std {
std::ostream &operator<<(std::ostream &os, std::vector<int> const &v);

template <class A, class B>
std::ostream &operator<<(std::ostream &os, std::pair<A, B> p) {
  os << "(" << p.first << ", " << p.second << ")";
  return os;
}
} // namespace std

namespace data {
/**
 *  A random list factory that yields edge cases first, then random
 *  To be used as a data generator with boost/test.
 *  Min and max are both inclusive.
 */
class rilist {
private:
  int min, max, minsize, maxsize;

public:
  rilist(int min_, int max_, int minsize_, int maxsize_)
      : min(min_), max(max_), minsize(minsize_), maxsize(maxsize_){};

  enum { arity = 1 };

  class iterator {
  public:
    iterator(int min_, int max_, int minsize_, int maxsize_);

    std::vector<int> operator*() const;
    void operator++();

  private:
    int min, max, minsize, maxsize;
    int currentsize, in_size_idx;
    std::vector<int> current_vector;

    std::vector<int> random_vector() const;
    std::vector<int> sequence_vector() const;
  };

  iterator begin() const;
  boost::unit_test::data::size_t size() const;
};

} // namespace data

template <>
struct boost::unit_test::data::monomorphic::is_dataset<data::rilist> {
  static const bool value = true;
};

#endif // UTILS_H_
