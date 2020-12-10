#ifndef __DIMENSION_H_
#define __DIMENSION_H_

namespace hypercubes {

class Dimension {
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
  Dimension(int s, int ht);

private:
  int starts[NUM_PORTIONS + 1];
};

} // namespace hypercubes

#endif // __DIMENSION_H_
