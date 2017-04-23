#include "rustfp/filter.h"
#include "rustfp/find.h"
#include "rustfp/find_map.h"
#include "rustfp/fold.h"
#include "rustfp/for_each.h"
#include "rustfp/iter.h"
#include "rustfp/map.h"
#include "rustfp/option.h"
#include "rustfp/range.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

// rustfp
using rustfp::filter;
using rustfp::find;
using rustfp::find_map;
using rustfp::fold;
using rustfp::for_each;
using rustfp::iter;
using rustfp::map;
using rustfp::range;
using rustfp::None;
using rustfp::Some;

// std
using std::accumulate;
using std::cbegin;
using std::cend;
using std::cout;
using std::plus;
using std::string;
using std::to_string;
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

class ComplexTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
    }
};

TEST_F(SimpleTest, Filter)
{
    int sum = 0;

    iter(intVec)
        | filter([](const auto value)
        {
            return value % 2 == 1;
        })

        | for_each([&sum](const auto value)
        {
            sum += value;
        });

    EXPECT_EQ(9, sum);
}

TEST_F(SimpleTest, Fold)
{
    static constexpr int FOLD_ACC = 10;

    const auto fold_sum = iter(intVec)
        | fold(FOLD_ACC, plus<int>());

    EXPECT_EQ(accumulate(cbegin(intVec), cend(intVec), FOLD_ACC), fold_sum);
}

TEST_F(SimpleTest, FindSome)
{
    static constexpr auto FIND_VALUE = 5;

    const auto find_some_opt = iter(intVec)
        | find([](const auto value) { return value == FIND_VALUE; });

    EXPECT_TRUE(find_some_opt.is_some());
    EXPECT_EQ(FIND_VALUE, find_some_opt.get_unchecked());
}

TEST_F(SimpleTest, FindNone)
{
    const auto find_none_opt = iter(intVec)
        | find([](const auto value) { return value == 6; });

    EXPECT_TRUE(find_none_opt.is_none());
}

TEST_F(SimpleTest, FindMapSome)
{
    static constexpr auto FIND_VALUE = 4;

    static const auto MAPPER_FN = [](const int value) { return value + 0.5; };

    const auto find_some_opt = iter(intVec)
        | find_map([](const auto value)
        {
            return value == FIND_VALUE
                ? Some(MAPPER_FN(value))
                : None;
        });

    EXPECT_TRUE(find_some_opt.is_some());
    EXPECT_EQ(MAPPER_FN(FIND_VALUE), find_some_opt.get_unchecked());
}

TEST_F(SimpleTest, FindMapNone)
{
    static constexpr auto FIND_VALUE = -1;

    static const auto MAPPER_FN = [](const int value) { return value + 0.5; };

    const auto find_some_opt = iter(intVec)
        | find_map([](const auto value)
        {
            return value == FIND_VALUE
                ? Some(MAPPER_FN(value))
                : None;
        });

    EXPECT_TRUE(find_some_opt.is_none());
}

TEST_F(SimpleTest, ForEach)
{
    int sum = 0;

    iter(intVec)
        | for_each([&sum](const auto value)
        {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(intVec), cend(intVec), 0), sum);
}

TEST_F(SimpleTest, Map)
{
    double sum = 0.0;

    iter(intVec)
        | map([](const auto value)
        {
            return value * 0.5;
        })

        | for_each([&sum](const auto value)
        {
            sum += value;
        });

    EXPECT_EQ(accumulate(cbegin(intVec), cend(intVec), 0) * 0.5, sum);
}

TEST_F(SimpleTest, Range)
{
    static constexpr int FOLD_ACC = 5;

    const auto sum = range(0, 6)
        | fold(FOLD_ACC, plus<int>());

    EXPECT_EQ(accumulate(cbegin(intVec), cend(intVec), FOLD_ACC), sum);
}

TEST_F(ComplexTest, FilterMapFold)
{
    const auto eleven_div_str = range(1, 100)
        | filter([](const auto value) { return value % 11 == 0; })
        | map([](const auto value) { return to_string(value); })
        | fold(string{}, [](const auto acc, const auto &rhs)
        {
            return acc + rhs + " ";
        });

    EXPECT_EQ("11 22 33 44 55 66 77 88 99 ", eleven_div_str);
}

TEST_F(ComplexTest, FilterMapFind)
{
    // .5 is a easily representable value in mantissa
    // so that the floating float comparison can directly compare the values
    static constexpr auto FIND_VALUE = 34.5;

    const auto find_opt = range(1, 100)
        | filter([](const auto value) { return value % 17 == 0; })
        | map([](const auto value) { return value + 0.5; })
        | find([](const auto value)
        {
            return value == FIND_VALUE;
        });

    EXPECT_TRUE(find_opt.is_some());
    EXPECT_EQ(FIND_VALUE, find_opt.get_unchecked());
}

int main(int argc, char * argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
