#define BOOST_TEST_MODULE test_partitioning1D
#include <boost/test/included/unit_test.hpp>

// We need to use a concrete class for testing.
#include "1D/q1D.hpp"
#include <vector>

using namespace hypercubes::slow;

struct Q1DFixture{

    Q1DPeriodic partitioning1D;

    Q1DFixture() : partitioning1D(SizeParity{42,Parity::EVEN}, /* sp */
                                  0, /*dimension*/
                                  "test1D", /*name*/
                                  4 /*nparts*/){};


};

/* Testing all non-virtual methods in Partitioning1D
 * */
BOOST_AUTO_TEST_SUITE(test_partitioning1D)

BOOST_FIXTURE_TEST_CASE(test_starts, Q1DFixture){

    std::vector<int> exp_starts{0,11,22,33};
    for(int i=0; i<exp_starts.size(); ++i)
        BOOST_TEST(exp_starts[i] == partitioning1D.start(i));

}

BOOST_FIXTURE_TEST_CASE(test_ends, Q1DFixture){
    std::vector<int> exp_ends{11,22,33,42};
    for(int i=0; i<exp_ends.size(); ++i)
        BOOST_TEST(exp_ends[i] == partitioning1D.end(i));
}


BOOST_AUTO_TEST_SUITE_END()
