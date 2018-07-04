#include <sample_anyiterator.hpp>

#include <sstream>
#include <forward_list>
#include <vector>

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
    // GIVEN
    std::string test("Hello, World!");
    sample::any_input_iterator<const char> first(begin(test));
    sample::any_input_iterator<const char> firstCopy(begin(test));
    sample::any_input_iterator<const char> last(end(test));

    // WHEN
    std::string test2(sample::any_input_iterator<const char>(begin(test)),
                      sample::any_input_iterator<const char>(end(test)));

    // THEN
    using namespace ::testing;
    EXPECT_THAT(first, Eq(firstCopy));
    EXPECT_THAT(first, Ne(last));
    EXPECT_THAT(test2, StrEq(test));
}

TEST(OutputIteratorTest, constructible_from_output_iterator)
{
    std::vector<char> v;
    sample::any_output_iterator<char> it(
        std::back_inserter(v));
}

TEST(OutputIteratorTest, iterator_works_as_expected)
{
    // GIVEN
    std::string test("Hello, World!");
    std::string test2;

    sample::any_output_iterator<char> output(
        std::back_inserter(test2));

    // WHEN
    std::copy(begin(test), end(test), output);

    // THEN
    using namespace ::testing;
    EXPECT_THAT(test2, StrEq(test));
}

TEST(ForwardIteratorTest, constructible_from_forward_iterator)
{
    std::forward_list<int> list;
    sample::any_forward_iterator<int> it(begin(list));
}

TEST(ForwardIteratorTest, iterator_works_as_expected)
{
    // GIVEN
    std::forward_list<int> list{1, 2, 3, 4, 5, 6, 7, 8};
    sample::any_forward_iterator<int> first(begin(list));
    sample::any_forward_iterator<int> firstCopy(first);
    sample::any_forward_iterator<int> last(end(list));

    // WHEN
    std::forward_list<int> list2{first, last};

    // THEN
    using namespace ::testing;
    EXPECT_THAT(first, Eq(firstCopy));
    EXPECT_THAT(first, Ne(last));
    EXPECT_THAT(list2, ContainerEq(list));
}
