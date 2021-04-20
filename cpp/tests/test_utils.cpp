#define BOOST_TEST_MODULE utils
#include <boost/test/included/unit_test.hpp>
#include "utils.hpp"
#include <vector>
#include <list>
#include <string>

BOOST_AUTO_TEST_SUITE(zip_cases)

BOOST_AUTO_TEST_CASE(TupleCutter_default_selector){

    auto t = std::make_tuple(1,"a",3,'b');
    auto tcut = TupleCutter<2>::cut(t);

    auto expected = std::make_tuple(1,"a",3);

    bool cond = expected == tcut;
    BOOST_TEST(cond);

}

BOOST_AUTO_TEST_CASE(TupleCutter_trivial_selector){

    auto trivial_selector = [](auto t){return t;};

    auto t = std::make_tuple(1,"a",3,'b');
    auto tcut = TupleCutter<2>::cut(t,trivial_selector);

    auto expected = std::make_tuple(1,"a",3);

    bool cond = expected == tcut;
    BOOST_TEST(cond);

}

BOOST_AUTO_TEST_CASE(TupleCutter_vector){

    std::vector<int> v0{1,2,3,4,5};
    std::vector<char> v1{'1','2','3','4','5'};
    std::vector<int> v2{6,7,8,9,10};
    std::vector<std::string> v3{"1","2","3","4","5"};

    auto vs = std::make_tuple(v0,v1,v2,v3);

    for(int i=0; i<5; i++){
        auto selector = [i](auto v){return v[i];};
        auto tcut = TupleCutter<2>::cut(vs,selector);

        auto expected = std::make_tuple(v0[i],v1[i],v2[i]);

        bool cond = expected == tcut;
        BOOST_TEST(cond);
    }

}

BOOST_AUTO_TEST_SUITE_END()
