#define BOOST_TEST_MODULE rilist_test
#include <boost/test/included/unit_test.hpp>
#include <set>

#include "random_int_list.hpp"

BOOST_AUTO_TEST_SUITE(rilist)

namespace bdata = boost::unit_test::data;
BOOST_AUTO_TEST_CASE(test_rilist){

  data::rilist generator(1, 10, 2, 3);
  auto it = generator.begin();

  std::set<std::vector<int>> results;
  int sequence_elements = 2*2 + 2*2*2;
  for(int i=0; i < sequence_elements; ++i,++it) results.insert(*it);

  BOOST_TEST(results.size() == sequence_elements);

  for(auto& el: results)
      for(auto& i: el)
          for(int a=2;a<10;++a)
              BOOST_TEST(i!=a);

}


BOOST_AUTO_TEST_SUITE_END()
