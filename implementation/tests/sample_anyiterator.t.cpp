#include <sample_anyiterator.hpp>

#include <sstream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

TEST(InputIteratorTest, constructible_from_input_iterator)
{
    std::stringstream s("Hello");
    sample::any_input_iterator<std::string> it(
        std::istream_iterator<std::string>(s));
}

TEST(InputIteratorTest, iterator_works_as_expected)
{
    using namespace ::testing;
    std::string test("Hello, World!");
    std::string test2(sample::any_input_iterator<const char>(begin(test)),
                      sample::any_input_iterator<const char>(end(test)));

    ASSERT_THAT(test2, StrEq(test));
}