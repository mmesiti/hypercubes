#include "selectors/intervals.hpp"
#include <boost/test/data/test_case.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <exception>
#include <set>

using namespace hypercubes::slow;
namespace bdata = boost::unit_test::data;

BOOST_AUTO_TEST_SUITE(test_interval)
BOOST_AUTO_TEST_CASE(test_same_interval_true) {
  Interval ab(3, 5);
  Interval cd(3, 5);
  BOOST_TEST(same(ab, cd));
}
BOOST_AUTO_TEST_CASE(test_same_interval_false) {
  Interval ab(3, 5);
  Interval cd(3, 4);
  Interval ef(2, 5);
  BOOST_TEST(not same(ab, cd));
  BOOST_TEST(not same(ab, ef));
}

BOOST_AUTO_TEST_CASE(test_interval_contains_true) {
  Interval ab(3, 5);
  BOOST_TEST(ab.contains(4));
}
BOOST_AUTO_TEST_CASE(test_interval_contains_false) {
  Interval ab(3, 5);
  BOOST_TEST(not ab.contains(5));
}

BOOST_AUTO_TEST_CASE(test_interval_and_overlap) {
  Interval ab(3, 5);
  Interval cd(4, 6);
  {
    auto cb = ab and cd;
    BOOST_TEST(cb.min == 4);
    BOOST_TEST(cb.max == 5);
  }
  {
    auto cb = cd and ab;
    BOOST_TEST(cb.min == 4);
    BOOST_TEST(cb.max == 5);
  }
}
BOOST_AUTO_TEST_CASE(test_interval_and_almost_touching) {
  Interval ab(3, 5);
  Interval cd(5, 7);
  auto e = ab and cd;
  BOOST_TEST(e.empty());
}
BOOST_AUTO_TEST_CASE(test_interval_and_disjoint) {
  Interval ab(-13, 5);
  Interval cd(14, 16);
  auto cb = ab and cd;
  BOOST_TEST(cb.empty());
}

BOOST_AUTO_TEST_CASE(test_interval_or_disjoint) {
  Interval ab(-13, 5);
  Interval cd(14, 16);
  auto abcd = ab or cd;
  BOOST_TEST(abcd.vs.size() == 2);
  BOOST_TEST(same(abcd.vs[0], ab));
  BOOST_TEST(same(abcd.vs[1], cd));
}
BOOST_AUTO_TEST_CASE(test_interval_or_disjoint_reverse) {
  Interval ab(-13, 5);
  Interval cd(14, 16);
  auto abcd = cd or ab;
  BOOST_TEST(abcd.vs.size() == 2);
  BOOST_TEST(same(abcd.vs[0], ab));
  BOOST_TEST(same(abcd.vs[1], cd));
}
BOOST_AUTO_TEST_CASE(test_interval_or_overlap) {
  Interval ab(3, 5);
  Interval cd(4, 6);
  auto abcd = ab or cd;
  BOOST_TEST(abcd.vs.size() == 1);
  BOOST_TEST(abcd.vs[0].min == ab.min);
  BOOST_TEST(abcd.vs[0].max == cd.max);
}
BOOST_DATA_TEST_CASE(test_interval_unary_minus_check, bdata::xrange(-10, 10),
                     i) {
  Interval ab(3, 5);
  auto cd = -ab;
  bool check = ((ab.contains(i) and cd.contains(-i)) or
                (not ab.contains(i) and not cd.contains(-i)));
  BOOST_TEST(check);
}
BOOST_AUTO_TEST_CASE(test_interval_div_positive) {
  Interval ab{-10, 15};
  int c = 2;

  Interval div = ab / c;

  int min = 100, max = -100;
  for (int i = -20; i < 20; ++i)
    if (ab.contains(i)) {
      min = std::min(min, i / c);
      max = std::max(max, i / c + 1);
    }
  BOOST_TEST(min == -5);
  BOOST_TEST(max == 7 + 1);
  BOOST_TEST(div.min == min);
  BOOST_TEST(div.max == max);
}
BOOST_AUTO_TEST_CASE(test_interval_div_negative) {
  Interval ab{-10, 15};
  int c = -2;

  Interval div = ab / c;

  int min = 100, max = -100;
  for (int i = -20; i < 20; ++i)
    if (ab.contains(i)) {
      min = std::min(min, i / c);
      max = std::max(max, i / c + 1);
    }
  BOOST_TEST(min == -7);
  BOOST_TEST(max == 5 + 1);
  BOOST_TEST(div.min == min);
  BOOST_TEST(div.max == max);
}

BOOST_AUTO_TEST_CASE(test_interval_less_disjoint_T) {
  Interval ab(3, 5);
  Interval cd(5, 8);
  BoolM check = ab < cd;
  BOOST_TEST(check == BoolM::T);
}
BOOST_AUTO_TEST_CASE(test_interval_less_disjoint_F) {
  Interval ab(3, 5);
  Interval cd(5, 8);
  BoolM check = cd < ab;
  BOOST_TEST(check == BoolM::F);
}
BOOST_AUTO_TEST_CASE(test_interval_less_overlap_M) {
  Interval ab(3, 6);
  Interval cd(5, 8);
  BoolM check = ab < cd;
  BOOST_TEST(check == BoolM::M);
}

#define INTERVAL_DATA_TEST_CASE(NAME, OP)                                      \
  BOOST_DATA_TEST_CASE(NAME,                                                   \
                       bdata::xrange(-6, 6) * bdata::xrange(0, 6) *            \
                           bdata::xrange(-6, 6) * bdata::xrange(0, 6),         \
                       a, d1, c, d2) {                                         \
    int b = a + d1;                                                            \
    int d = c + d2;                                                            \
    Interval ab(a, b);                                                         \
    Interval cd(c, d);                                                         \
    int count = 0, count_not = 0;                                              \
    for (int x = a; x < b; x++)                                                \
      for (int y = c; y < d; y++) {                                            \
        if (x OP y)                                                            \
          count++;                                                             \
        else                                                                   \
          count_not++;                                                         \
      }                                                                        \
    BoolM expected;                                                            \
    if (count == 0 and count_not > 0)                                          \
      expected = BoolM::F;                                                     \
    else if (count_not == 0 and count > 0)                                     \
      expected = BoolM::T;                                                     \
    else                                                                       \
      expected = BoolM::M;                                                     \
    auto check = ab OP cd;                                                     \
    std::cout << "count true: " << count << " count false: " << count_not      \
              << " " << a << " " << b << " " << c << " " << d << std::endl;    \
    BOOST_TEST(check == expected);                                             \
  }

#define INTERVAL_INT_DATA_TEST_CASE(NAME, OP)                                  \
  BOOST_DATA_TEST_CASE(                                                        \
      NAME, bdata::xrange(-6, 6) * bdata::xrange(0, 6) * bdata::xrange(-6, 6), \
      a, d1, c) {                                                              \
    int b = a + d1;                                                            \
    Interval ab(a, b);                                                         \
                                                                               \
    int count = 0, count_not = 0;                                              \
    for (int x = a; x < b; x++) {                                              \
      if (x OP c)                                                              \
        count++;                                                               \
      else                                                                     \
        count_not++;                                                           \
    }                                                                          \
    BoolM expected;                                                            \
    if (count == 0 and count_not > 0)                                          \
      expected = BoolM::F;                                                     \
    else if (count_not == 0 and count > 0)                                     \
      expected = BoolM::T;                                                     \
    else if (count_not == 0 and count == 0)                                    \
      expected = BoolM::F;                                                     \
    else                                                                       \
      expected = BoolM::M;                                                     \
    auto check = ab OP c;                                                      \
    BOOST_TEST(check == expected);                                             \
  }

#define INT_INTERVAL_DATA_TEST_CASE(NAME, OP)                                  \
  BOOST_DATA_TEST_CASE(                                                        \
      NAME, bdata::xrange(-6, 6) * bdata::xrange(0, 6) * bdata::xrange(-6, 6), \
      a, d1, c) {                                                              \
    int b = a + d1;                                                            \
    Interval ab(a, b);                                                         \
                                                                               \
    int count = 0, count_not = 0;                                              \
    for (int x = a; x < b; x++) {                                              \
      if (c OP x)                                                              \
        count++;                                                               \
      else                                                                     \
        count_not++;                                                           \
    }                                                                          \
    BoolM expected;                                                            \
    if (count == 0 and count_not > 0)                                          \
      expected = BoolM::F;                                                     \
    else if (count_not == 0 and count > 0)                                     \
      expected = BoolM::T;                                                     \
    else if (count_not == 0 and count == 0)                                    \
      expected = BoolM::F;                                                     \
    else                                                                       \
      expected = BoolM::M;                                                     \
    auto check = c OP ab;                                                      \
    BOOST_TEST(check == expected);                                             \
  }

// < family
INTERVAL_DATA_TEST_CASE(test_interval_less, <)
INT_INTERVAL_DATA_TEST_CASE(test_int_less_interval, <)
INTERVAL_INT_DATA_TEST_CASE(test_interval_less_int, <)

// == family
INTERVAL_DATA_TEST_CASE(test_interval_equal, ==)
INT_INTERVAL_DATA_TEST_CASE(test_int_equal_interval, ==)
INTERVAL_INT_DATA_TEST_CASE(test_interval_equal_int, ==)

// <= family
INTERVAL_DATA_TEST_CASE(test_interval_lessequal, <=)
INT_INTERVAL_DATA_TEST_CASE(test_int_lessequal_interval, <=)
INTERVAL_INT_DATA_TEST_CASE(test_interval_lessequal_int, <=)

// > family
INTERVAL_DATA_TEST_CASE(test_interval_greater, >)
INT_INTERVAL_DATA_TEST_CASE(test_int_greater_interval, >)
INTERVAL_INT_DATA_TEST_CASE(test_interval_greater_int, >)

// >= family
INTERVAL_DATA_TEST_CASE(test_interval_greaterequal, >=)
INT_INTERVAL_DATA_TEST_CASE(test_int_greaterequal_interval, >=)
INTERVAL_INT_DATA_TEST_CASE(test_interval_greaterequal_int, >=)

// != family
INTERVAL_DATA_TEST_CASE(test_interval_notequal, !=)
INT_INTERVAL_DATA_TEST_CASE(test_int_notequal_interval, !=)
INTERVAL_INT_DATA_TEST_CASE(test_interval_notequal_int, !=)

/*
BOOST_DATA_TEST_CASE(test_interval_less,
                     bdata::xrange(-6, 6) *     //
                         bdata::xrange(0, 6) *  //
                         bdata::xrange(-6, 6) * //
                         bdata::xrange(0, 6),
                     a, d1, c, d2) {
  int b = a + d1;
  int d = c + d2;
  Interval ab(a, b);
  Interval cd(c, d);

  int count_less = 0, count_not_less = 0;
  for (int x = a; x < b; x++)
    for (int y = c; y < d; y++) {
      if (x < y)
        count_less++;
      else
        count_not_less++;
    }
  BoolM expected;
  if (count_less == 0 and count_not_less > 0)
    expected = BoolM::F;
  else if (count_not_less == 0 and count_less > 0)
    expected = BoolM::T;
  else
    expected = BoolM::M;

  auto check = ab < cd;
  std::cout << "less " << count_less << " not less " << count_not_less << " "
            << a << " " << b << " " << c << " " << d << std::endl;
  BOOST_TEST(check == expected);
}

BOOST_DATA_TEST_CASE(test_interval_less_int,
                     bdata::xrange(-6, 6) *    //
                         bdata::xrange(0, 6) * //
                         bdata::xrange(-6, 6),
                     a, d1, c) {
  int b = a + d1;
  Interval ab(a, b);

  int count_less = 0, count_not_less = 0;
  for (int x = a; x < b; x++) {
    if (x < c)
      count_less++;
    else
      count_not_less++;
  }
  BoolM expected;
  if (count_less == 0 and count_not_less > 0)
    expected = BoolM::F;
  else if (count_not_less == 0 and count_less > 0)
    expected = BoolM::T;
  else if (count_not_less == 0 and count_less == 0)
    // This means that there is no number in the interval
    // for which the condition can be true.
    expected = BoolM::F;
  else
    expected = BoolM::M;

  auto check = ab < c;
  std::cout << "less " << count_less << " not less " << count_not_less << " "
            << a << " " << b << " " << c << std::endl;
  BOOST_TEST(check == expected);
}
BOOST_DATA_TEST_CASE(test_int_less_interval,
                     bdata::xrange(-6, 6) *    //
                         bdata::xrange(0, 6) * //
                         bdata::xrange(-6, 6),
                     a, d1, c) {
  int b = a + d1;
  Interval ab(a, b);

  int count_less = 0, count_not_less = 0;
  for (int x = a; x < b; x++) {
    if (c < x)
      count_less++;
    else
      count_not_less++;
  }
  BoolM expected;
  if (count_less == 0 and count_not_less > 0)
    expected = BoolM::F;
  else if (count_not_less == 0 and count_less > 0)
    expected = BoolM::T;
  else if (count_not_less == 0 and count_less == 0)
    // This means that there is no number in the interval
    // for which the condition can be true.
    expected = BoolM::F;
  else
    expected = BoolM::M;

  auto check = c < ab;
  std::cout << "less " << count_less << " not less " << count_not_less << " "
            << a << " " << b << " " << c << std::endl;
  BOOST_TEST(check == expected);
}
BOOST_DATA_TEST_CASE(test_interval_equal,
                     bdata::xrange(-6, 6) *     //
                         bdata::xrange(0, 6) *  //
                         bdata::xrange(-6, 6) * //
                         bdata::xrange(0, 6),
                     a, d1, c, d2) {
  int b = a + d1;
  int d = c + d2;
  Interval ab(a, b);
  Interval cd(c, d);

  int count_equal = 0, count_not_equal = 0;
  for (int x = a; x < b; x++)
    for (int y = c; y < d; y++) {
      if (x == y)
        count_equal++;
      else
        count_not_equal++;
    }
  BoolM expected;
  if (count_equal == 0 and count_not_equal > 0)
    expected = BoolM::F;
  else if (count_not_equal == 0 and count_equal > 0)
    expected = BoolM::T;
  else
    expected = BoolM::M;

  auto check = ab == cd;
  std::cout << "less " << count_equal << " not less " << count_not_equal << " "
            << a << " " << b << " " << c << " " << d << std::endl;
  BOOST_TEST(check == expected);
}

BOOST_DATA_TEST_CASE(test_interval_lessequal,
                     bdata::xrange(-6, 6) *     //
                         bdata::xrange(0, 6) *  //
                         bdata::xrange(-6, 6) * //
                         bdata::xrange(0, 6),
                     a, d1, c, d2) {
  int b = a + d1;
  int d = c + d2;
  Interval ab(a, b);
  Interval cd(c, d);

  int count_lesseq = 0, count_not_lesseq = 0;
  for (int x = a; x < b; x++)
    for (int y = c; y < d; y++) {
      if (x <= y)
        count_lesseq++;
      else
        count_not_lesseq++;
    }
  BoolM expected;
  if (count_lesseq == 0 and count_not_lesseq > 0)
    expected = BoolM::F;
  else if (count_not_lesseq == 0 and count_lesseq > 0)
    expected = BoolM::T;
  else
    expected = BoolM::M;

  auto check = ab <= cd;
  std::cout << "<= :" << check << " <: " << (ab < cd) << " ==:" << (ab == cd)
            << "  ";
  std::cout << a << " " << b << " " << c << " " << d << std::endl;
  BOOST_TEST(check == expected);
}
BOOST_DATA_TEST_CASE(test_interval_lessequal_int,
                     bdata::xrange(-6, 6) *    //
                         bdata::xrange(0, 6) * //
                         bdata::xrange(-6, 6),
                     a, d1, c) {
  int b = a + d1;
  Interval ab(a, b);

  int count_less = 0, count_not_less = 0;
  for (int x = a; x < b; x++) {
    if (x <= c)
      count_less++;
    else
      count_not_less++;
  }
  BoolM expected;
  if (count_less == 0 and count_not_less > 0)
    expected = BoolM::F;
  else if (count_not_less == 0 and count_less > 0)
    expected = BoolM::T;
  else if (count_not_less == 0 and count_less == 0)
    // This means that there is no number in the interval
    // for which the condition can be true.
    expected = BoolM::F;
  else
    expected = BoolM::M;

  auto check = ab <= c;
  std::cout << "less " << count_less << " not less " << count_not_less << " "
            << a << " " << b << " " << c << std::endl;
  BOOST_TEST(check == expected);
}
BOOST_DATA_TEST_CASE(test_int_lessequal_interval,
                     bdata::xrange(-6, 6) *    //
                         bdata::xrange(0, 6) * //
                         bdata::xrange(-6, 6),
                     a, d1, c) {
  int b = a + d1;
  Interval ab(a, b);

  int count_less = 0, count_not_less = 0;
  for (int x = a; x < b; x++) {
    if (c <= x)
      count_less++;
    else
      count_not_less++;
  }
  BoolM expected;
  if (count_less == 0 and count_not_less > 0)
    expected = BoolM::F;
  else if (count_not_less == 0 and count_less > 0)
    expected = BoolM::T;
  else if (count_not_less == 0 and count_less == 0)
    // This means that there is no number in the interval
    // for which the condition can be true.
    expected = BoolM::F;
  else
    expected = BoolM::M;

  auto check = c <= ab;
  std::cout << "less " << count_less << " not less " << count_not_less << " "
            << a << " " << b << " " << c << std::endl;
  BOOST_TEST(check == expected);
}

BOOST_DATA_TEST_CASE(test_interval_greater,
                     bdata::xrange(-6, 6) *     //
                         bdata::xrange(0, 6) *  //
                         bdata::xrange(-6, 6) * //
                         bdata::xrange(0, 6),
                     a, d1, c, d2) {
  int b = a + d1;
  int d = c + d2;
  Interval ab(a, b);
  Interval cd(c, d);

  int count_less = 0, count_not_less = 0;
  for (int x = a; x < b; x++)
    for (int y = c; y < d; y++) {
      if (x > y)
        count_less++;
      else
        count_not_less++;
    }
  BoolM expected;
  if (count_less == 0 and count_not_less > 0)
    expected = BoolM::F;
  else if (count_not_less == 0 and count_less > 0)
    expected = BoolM::T;
  else
    expected = BoolM::M;

  auto check = ab > cd;
  std::cout << "less " << count_less << " not less " << count_not_less << " "
            << a << " " << b << " " << c << " " << d << std::endl;
  BOOST_TEST(check == expected);
}
BOOST_DATA_TEST_CASE(test_interval_greater_int,
                     bdata::xrange(-6, 6) *    //
                         bdata::xrange(0, 6) * //
                         bdata::xrange(-6, 6),
                     a, d1, c) {
  int b = a + d1;
  Interval ab(a, b);

  int count_less = 0, count_not_less = 0;
  for (int x = a; x < b; x++) {
    if (x > c)
      count_less++;
    else
      count_not_less++;
  }
  BoolM expected;
  if (count_less == 0 and count_not_less > 0)
    expected = BoolM::F;
  else if (count_not_less == 0 and count_less > 0)
    expected = BoolM::T;
  else if (count_not_less == 0 and count_less == 0)
    // This means that there is no number in the interval
    // for which the condition can be true.
    expected = BoolM::F;
  else
    expected = BoolM::M;

  auto check = ab > c;
  std::cout << "less " << count_less << " not less " << count_not_less << " "
            << a << " " << b << " " << c << std::endl;
  BOOST_TEST(check == expected);
}
BOOST_DATA_TEST_CASE(test_int_greater_interval,
                     bdata::xrange(-6, 6) *    //
                         bdata::xrange(0, 6) * //
                         bdata::xrange(-6, 6),
                     a, d1, c) {
  int b = a + d1;
  Interval ab(a, b);

  int count_less = 0, count_not_less = 0;
  for (int x = a; x < b; x++) {
    if (c > x)
      count_less++;
    else
      count_not_less++;
  }
  BoolM expected;
  if (count_less == 0 and count_not_less > 0)
    expected = BoolM::F;
  else if (count_not_less == 0 and count_less > 0)
    expected = BoolM::T;
  else if (count_not_less == 0 and count_less == 0)
    // This means that there is no number in the interval
    // for which the condition can be true.
    expected = BoolM::F;
  else
    expected = BoolM::M;

  auto check = c > ab;
  std::cout << "less " << count_less << " not less " << count_not_less << " "
            << a << " " << b << " " << c << std::endl;
  BOOST_TEST(check == expected);
}

*/
// Multiple interval operations

BOOST_AUTO_TEST_CASE(test_interval_not) {
  Interval v(3, 5);
  Intervals noti = not v;
  BOOST_TEST(noti.vs[0].min == -Interval::LIMIT);
  BOOST_TEST(noti.vs[0].max == 2);
  BOOST_TEST(noti.vs[1].min == 6);
  BOOST_TEST(noti.vs[1].max == Interval::LIMIT);
}
BOOST_AUTO_TEST_CASE(test_same_intervals_true) {
  Intervals abcd1({{3, 5}, {14, 16}});
  Intervals abcd2({{3, 5}, {14, 16}});
  BOOST_TEST(same(abcd1, abcd2));
}
BOOST_AUTO_TEST_CASE(test_same_intervals_false) {
  {
    Intervals abcd1({{3, 5}, {14, 16}, {21, 22}});
    Intervals abcd2({{3, 5}, {14, 16}});
    BOOST_TEST(not same(abcd1, abcd2));
  }
  {
    Intervals abcd1({{3, 5}, {14, 16}});
    Intervals abcd2({{3, 5}, {14, 17}});
    BOOST_TEST(not same(abcd1, abcd2));
  }
}
BOOST_AUTO_TEST_CASE(test_intervals_constructor_ordering_throws) {
  std::vector<Interval> v{{1, 2}, {2, 3}};
  BOOST_CHECK_THROW(Intervals is(v), std::invalid_argument);
}
BOOST_AUTO_TEST_CASE(test_intervals_contain_true) {
  Intervals abs({{3, 5}, {7, 9}});
  BOOST_TEST(abs.contain(8));
}
BOOST_AUTO_TEST_CASE(test_intervals_contain_false) {
  Intervals abs({{3, 5}, {7, 9}});
  BOOST_TEST(not abs.contain(9));
}

BOOST_AUTO_TEST_CASE(test_intervals_constructor_empty) {
  Intervals v;
  BOOST_TEST(v.vs.size() == 0);
}
BOOST_AUTO_TEST_CASE(test_interval_ab) {
  Interval v({4, 6});
  BOOST_TEST(v.min == 4);
  BOOST_TEST(v.max == 6);
}
BOOST_AUTO_TEST_CASE(test_intervals_ab) {
  Intervals v({{4, 6}});
  BOOST_TEST(v.vs.size() == 1);
  BOOST_TEST(v.vs[0].min == 4);
  BOOST_TEST(v.vs[0].max == 6);
}

BOOST_AUTO_TEST_CASE(test_intervals_or) {
  Intervals abcd({{3, 5}, {10, 25}});
  Intervals efgh({{4, 7}, {40, 45}});
  Intervals afcdgh({{3, 7}, {10, 25}, {40, 45}});
  auto out = abcd or efgh;
  BOOST_TEST(out.vs.size() == 3);
  BOOST_TEST(same(out, afcdgh));
}
BOOST_AUTO_TEST_CASE(test_intervals_and) {
  Intervals abcd({{3, 5}, {10, 25}});
  Intervals efgh({{4, 7}, {40, 45}});
  Intervals afcdgh({{4, 5}});
  auto out = abcd and efgh;
  BOOST_TEST(out.vs.size() == 1);
  BOOST_TEST(same(out, afcdgh));
}
BOOST_AUTO_TEST_CASE(test_intervals_not) {
  Intervals abcd({{3, 5}, {10, 25}});
  auto out = not abcd;
  Intervals outexp({{-Interval::LIMIT, 3}, //
                    {5, 10},               //
                    {25, Interval::LIMIT}});
  BOOST_TEST(out.vs.size() == 3);
  BOOST_TEST(same(out, outexp));
}
BOOST_AUTO_TEST_CASE(test_intervals_not_not) {
  Intervals abcd({{3, 5}, {10, 25}});
  auto out = not(not abcd);
  BOOST_TEST(same(out, abcd));
}

BOOST_AUTO_TEST_CASE(test_intervals_or_idempotent) {
  Intervals ivs({{-Interval::LIMIT, 3}, //
                 {5, 10},               //
                 {25, Interval::LIMIT}});
  BOOST_TEST(same(ivs, ivs or ivs));
}
BOOST_AUTO_TEST_CASE(test_intervals_and_idempotent) {
  Intervals ivs({{-Interval::LIMIT, 3}, //
                 {5, 10},               //
                 {25, Interval::LIMIT}});
  BOOST_TEST(same(ivs, ivs and ivs));
}

BOOST_DATA_TEST_CASE(test_intervals_unary_minus_check, bdata::xrange(-20, 20),
                     i) {
  Intervals ab({{3, 5}, {7, 10}});
  auto cd = -ab;
  bool check = ((ab.contain(i) and cd.contain(-i)) or
                (not ab.contain(i) and not cd.contain(-i)));
}

BOOST_AUTO_TEST_CASE(test_interval_plus) {
  Interval ab{3, 5};
  Interval cd{4, 6};

  Interval sum = ab + cd;

  std::set<int> sumset;
  for (int i = -20; i < 20; ++i)
    for (int j = -20; j < 20; ++j)
      if (ab.contains(i) and cd.contains(j))
        sumset.insert(i + j);

  for (int s = -50; s < 50; ++s) {
    bool check = (sum.contains(s) and sumset.count(s) != 0) or
                 (not sum.contains(s) and sumset.count(s) == 0);
    BOOST_TEST(check);
  }
}
BOOST_AUTO_TEST_CASE(test_interval_minus) {
  Interval ab{3, 5};
  Interval cd{4, 6};

  Interval sum = ab - cd;

  std::set<int> sumset;
  for (int i = -20; i < 20; ++i)
    for (int j = -20; j < 20; ++j)
      if (ab.contains(i) and cd.contains(j))
        sumset.insert(i - j);

  for (int s = -50; s < 50; ++s) {
    bool check = (sum.contains(s) and sumset.count(s) != 0) or
                 (not sum.contains(s) and sumset.count(s) == 0);
    BOOST_TEST(check);
  }
}
BOOST_AUTO_TEST_CASE(test_interval_mult) {
  Interval ab{3, 5};
  Interval cd{4, 6};

  Interval prod = ab * cd;

  int min = 100, max = -100;
  for (int i = -20; i < 20; ++i)
    for (int j = -20; j < 20; ++j)
      if (ab.contains(i) and cd.contains(j)) {
        min = std::min(min, i * j);
        max = std::max(max, i * j + 1);
      }
  BOOST_TEST(prod.min == min);
  BOOST_TEST(prod.max == max);
}

BOOST_AUTO_TEST_SUITE_END();
