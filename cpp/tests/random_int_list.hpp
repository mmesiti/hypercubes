#ifndef RANDOM_INT_LIST_H_
#define RANDOM_INT_LIST_H_

#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
#include <boost/test/included/unit_test.hpp>
#include <cmath>
#include <cstdlib>
#include <vector>

namespace data {
/**
 *  A random list factory that yields edge cases first, then random
 *  To be used as a data generator with boost/test.
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
    iterator(int min_, int max_, int minsize_, int maxsize_)
        : min(min_), max(max_), minsize(minsize_), maxsize(maxsize_),
          currentsize(minsize_), in_size_idx(0){};

    auto operator*() const {
      if (currentsize > maxsize)
        return this->random_vector();
      else
        return this->sequence_vector();
    }

    void operator++() {
      if (currentsize <= maxsize) {
        int max_idx = std::pow(2, currentsize);
        in_size_idx++;
        if (in_size_idx >= max_idx) {
          currentsize++;
          in_size_idx = 0;
        }
      }
    }

  private:
    int min, max, minsize, maxsize;
    int currentsize, in_size_idx;

    std::vector<int> random_vector() const {
      std::vector<int> res;
      int random_size = (minsize + rand()) % (maxsize + 1);
      for (int s = 0; s < random_size; s++) {
        int random_value = (min + rand()) % (max + 1);
        res.push_back(random_value);
      }
      return res;
    }

    std::vector<int> sequence_vector() const {
      std::vector<int> res;
      int idx = in_size_idx;
      for (int s = 0; s < currentsize; s++) {
        res.push_back(idx % 2 ? min : max);
        idx /= 2;
      }
      return res;
    }
  };

  iterator begin() const { return iterator(min, max, minsize, maxsize); }
  boost::unit_test::data::size_t size() const {
    return boost::unit_test::data::BOOST_TEST_DS_INFINITE_SIZE;
  }
};

} // namespace data

template <>
struct boost::unit_test::data::monomorphic::is_dataset<data::rilist> {
  static const bool value = true;
};

#endif // RANDOM_INT_LIST_H_
