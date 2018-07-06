#include <sample_anyiterator.hpp>

#include <sstream>
#include <forward_list>
#include <list>
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

TEST(BidirectionalIteratorTest, constructible_from_bidirectional_iterator)
{
    std::list<int> list;
    sample::any_bidirectional_iterator<int> it(begin(list));
}

TEST(BidirectionalIteratorTest, iterator_works_as_expected)
{
    // GIVEN
    std::list<int> list{1, 2, 3, 4, 5};
    sample::any_bidirectional_iterator<int> first(begin(list));
    sample::any_bidirectional_iterator<int> last(end(list));

    // WHEN
    std::list<int> list2{first, last};

    // THEN
    using namespace ::testing;
    EXPECT_THAT(*first++, Eq(1));
    EXPECT_THAT(*first--, Eq(2));
    EXPECT_THAT(*first, Eq(1));
    EXPECT_THAT(list2, ContainerEq(list));
}

TEST(RandomAccessIteratorTest, constructible_from_random_access_iterator)
{
    std::array<int, 3u> arr{1, 2, 3};
    sample::any_random_access_iterator<int> first(begin(arr));
}

TEST(RandomAccessIteratorTest, iterator_works_as_expected)
{
    // GIVEN
    std::array<int, 5u> arr{1, 2, 3, 4, 5};
    sample::any_random_access_iterator<int> first(begin(arr));
    sample::any_random_access_iterator<int> last(end(arr));

    // WHEN
    std::vector<int> v{first, last};
    auto copy = first;
    copy += 3;
    copy -= 1;

    // THEN
    using namespace ::testing;
    EXPECT_THAT(*first, Eq(1));
    EXPECT_THAT(*copy, Eq(3));
    EXPECT_THAT(v, ElementsAreArray(cbegin(arr), cend(arr)));
}

TEST(RandomAccessIteratorTest, convertible_to_bidirectional_iterator)
{
        // GIVEN
    std::array<int, 5u> arr{1, 2, 3, 4, 5};
    sample::any_random_access_iterator<int> first(begin(arr));
    sample::any_random_access_iterator<int> last(end(arr));

    // WHEN
    sample::any_bidirectional_iterator<int> weaker(first);

    // THEN
    using namespace ::testing;
    EXPECT_THAT(*weaker, Eq(1));
}

TEST(RandomAccessIterator, user_defined_deduction_guide_works)
{
    // GIVEN
    std::vector<int> v{1, 2, 3};

    // WHEN
    sample::any_iterator it(begin(v));

    // THEN
    using namespace ::testing;
    EXPECT_THAT(std::is_same_v<decltype(it),
        sample::any_random_access_iterator<int, int&, int*>>,
        Eq(true));
}
