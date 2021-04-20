#ifndef __DIMENSION_H_
#define __DIMENSION_H_
#include <vector>

namespace hypercubes {

// could be also list?
template <typename T> using container = std::vector<T>;

class Side {
public:
  enum Portion {
    HALO_MINUS,
    BORDER_MINUS,
    BULK,
    BORDER_PLUS,
    HALO_PLUS,
    NUM_PORTIONS
  };
  int size, halo_thickness;
  int start(Portion p);
  int end(Portion p);
  Portion get_portion(int x);
  Side(int s, int ht);

private:
  int starts[NUM_PORTIONS + 1];
};

struct Factor {
  int f;
  Factor(int _f);
  Factor operator*(Factor);
};

class Dimension {
  container<Side> sides;
  Factor factor(int i);
  Dimension(container<Side> _sides);
};

} // namespace hypercubes

#endif // __DIMENSION_H_
