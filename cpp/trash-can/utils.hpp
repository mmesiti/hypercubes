#ifndef UTILS_H_
#define UTILS_H_

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>

namespace data {

class minmax {
public:
  minmax(int min_, int max_) : min(min_), max(max_) {}
  enum { arity = 1 };

  class iterator {
  private:
    int min, max, c;

  public:
    iterator(int min_, int max_) : min(min_), max(max_), c(0){};
    int operator*() const {
      if (c == 0) {
        return min;
      } else {
        return max;
      }
    }
    void operator++() {
      if (c < 2)
        c++;
    }
  };

  iterator begin() const { return iterator(min, max); }
  boost::unit_test::data::size_t size() const { return 2; }

private:
  int min, max;
};

} // namespace data

template <>
struct boost::unit_test::data::monomorphic::is_dataset<data::minmax> {
  static const bool value = true;
};

#endif // UTILS_H_
