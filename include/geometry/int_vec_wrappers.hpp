#ifndef INT_VEC_WRAPPERS_H_
#define INT_VEC_WRAPPERS_H_
#include <ostream>
#include <vector>

namespace hypercubes {
namespace slow {
class Coordinates;
class Indices;
} // namespace slow
} // namespace hypercubes
namespace std {
std::ostream &operator<<(std::ostream &os,
                         const hypercubes::slow::Coordinates &cs);
std::ostream &operator<<(std::ostream &os,
                         const hypercubes::slow::Indices &idxs);
} // namespace std

namespace hypercubes {
namespace slow {

using std::vector;
#define INTVEC_BOILERPLATE(CLASSNAME, WRAPPEDNAME)                             \
private:                                                                       \
  vector<int> WRAPPEDNAME;                                                     \
                                                                               \
public:                                                                        \
  int &operator[](size_t d);                                                   \
  int operator[](size_t d) const;                                              \
  template <class... Args> CLASSNAME(Args... args) : WRAPPEDNAME(args...){};   \
  CLASSNAME(std::initializer_list<int> il);                                    \
  size_t size() const;                                                         \
  friend std::ostream &std::operator<<(std::ostream &os,                       \
                                       const hypercubes::slow::CLASSNAME &cs); \
  auto begin() { return WRAPPEDNAME.begin(); }                                 \
  auto begin() const { return WRAPPEDNAME.begin(); }                           \
  auto end() { return WRAPPEDNAME.end(); }                                     \
  auto end() const { return WRAPPEDNAME.end(); }                               \
  bool operator==(const CLASSNAME &other) const;                               \
  bool operator!=(const CLASSNAME &other) const;

// Coordinates are for locations in physical space - e.g, 4D
class Coordinates {
  INTVEC_BOILERPLATE(Coordinates, coords);
};
// Indices are for location in the partitioning tree
// (not that different from coordinates, after all,
// but in a space with a different dimensionality
// and some other constraint)
class Indices {
  INTVEC_BOILERPLATE(Indices, idxs)

  void push_back(int);
  friend Indices append(int, const Indices &);
  friend Indices tail(const Indices &);
};
#undef INTVEC_BOILERPLATE

Indices append(Indices, int);
Indices append(int, const Indices &);
Indices tail(const Indices &);

} // namespace slow
} // namespace hypercubes
#endif // INT_VEC_WRAPPERS_H_
