#include "rustfp/filter.h"
#include "rustfp/fold.h"
#include "rustfp/for_each.h"
#include "rustfp/iter.h"
#include "rustfp/map.h"

#include "gtest/gtest.h"

#include <iostream>
#include <iterator>
#include <numeric>
#include <vector>

// rustfp
using rustfp::filter;
using rustfp::for_each;
using rustfp::iter;
using rustfp::map;

// std
using std::accumulate;
using std::cbegin;
using std::cend;
using std::cout;
using std::vector;

class SimpleTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        intVec = vector<int>{0, 1, 2, 3, 4, 5};
    }

    vector<int> intVec;
};

TEST_F(SimpleTest, Filter)
{
    int sum = 0;

    iter(intVec)
        | filter([](const int value)
        {
            return value % 2 == 1;
        })

        | for_each([&sum](const int value)
        {
            sum += value;
        });

    EXPECT_EQ(9, sum);
}

TEST_F(SimpleTest, ForEach)
{
    int sum = 0;

    iter(intVec)
        | for_each([&sum](const int value)
        {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(intVec), cend(intVec), 0), sum);
}

TEST_F(SimpleTest, Map)
{
    double sum = 0.0;

    iter(intVec)
        | map([](const int value)
        {
            return value * 0.5;
        })

        | for_each([&sum](const double value)
        {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(intVec), cend(intVec), 0) * 0.5, sum);
}

int main(int argc, char * argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
