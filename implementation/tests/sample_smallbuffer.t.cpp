#include <sample_smallbuffer.hpp>

#include <array>
#include <iostream>
#include <tuple>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace test {
    struct TestBase {
        int i;
        int j;
        int k;

        TestBase(int i, int j, int k) : i(i), j(j), k(k) {}
        virtual ~TestBase() = default;

        constexpr bool operator==(const TestBase& rhs) const noexcept {
            return std::tie(i, j, k) == std::tie(rhs.i, rhs.j, rhs.k);
        }

        friend std::ostream& operator<<(std::ostream& os, const TestBase& rhs)
        {
            const auto [i, j, k] = rhs;
            return os << "{" << i << ", " << j << ", " << k << "}";
        }
    };

    struct TestDerived : TestBase {
        int a;
        int b;
        int c;

        TestDerived(int i, int j, int k, int a, int b, int c) :
            TestBase(i, j, k), a(a), b(b), c(c) {}

        constexpr bool operator==(const TestDerived& rhs) const noexcept {
            return std::tie(i, j, k, a, b, c) == 
                std::tie(rhs.i, rhs.j, rhs.k, rhs.a, rhs.b, rhs.c);
        }

        friend std::ostream& operator<<(std::ostream& os, const TestDerived& rhs)
        {
            return os << "{{" << rhs.i << ", " << rhs.j << ", " << rhs.k << "}, "
                      << rhs.a << ", " << rhs.b << ", " << rhs.c << "}";
        }
    };
} // close namespace test

TEST(SmallBuffer, constructible_with_small_base)
{
    // GIVEN
    test::TestBase test{1, 2, 3};

    // WHEN
    using Buffer = sample::detail::SmallBuffer<test::TestBase, sizeof(test)>;
    Buffer buffer(std::in_place_type<decltype(test)>, test);

    // THEN
    using namespace ::testing;
    ASSERT_THAT(*buffer, Eq(test));
}

TEST(SmallBuffer, constructible_with_large_base)
{
    // GIVEN
    test::TestBase test{1, 2, 3};

    // WHEN
    static_assert(sizeof(test) > sizeof(2 * sizeof(int)));
    using Buffer = sample::detail::SmallBuffer<test::TestBase, 
        sizeof(test) - (2u * sizeof(int))>;
    Buffer buffer(std::in_place_type<decltype(test)>, test);

    // THEN
    using namespace ::testing;
    ASSERT_THAT(*buffer, Eq(test));
}

TEST(SmallBuffer, small_base_copyable)
{
    // GIVEN
    test::TestBase test{1, 2, 3};

    // WHEN
    using Buffer = sample::detail::SmallBuffer<test::TestBase, sizeof(test)>;
    Buffer buffer(std::in_place_type<decltype(test)>, test);
    Buffer buffer2(buffer);

    // THEN
    using namespace ::testing;
    ASSERT_THAT(*buffer2, Eq(test));
}

TEST(SmallBuffer, small_base_movable)
{
    // GIVEN
    test::TestBase test{1, 2, 3};

    // WHEN
    using Buffer = sample::detail::SmallBuffer<test::TestBase, sizeof(test)>;
    Buffer buffer(std::in_place_type<decltype(test)>, test);
    Buffer buffer2(
        std::move(buffer));

    // THEN
    using namespace ::testing;
    ASSERT_THAT(*buffer2, Eq(test));
}

TEST(SmallBuffer, large_base_copyable)
{
    // GIVEN
    test::TestBase test{1, 2, 3};

    // WHEN
    static_assert(sizeof(test) > sizeof(2 * sizeof(int)));
    using BufferType = sample::detail::SmallBuffer<test::TestBase, 
        sizeof(test) - (2u * sizeof(int))>;
    BufferType buffer(std::in_place_type<decltype(test)>, test);
    BufferType buffer2(buffer);

    // THEN
    using namespace ::testing;
    ASSERT_THAT(*buffer2, Eq(test));
}

TEST(SmallBuffer, large_base_movable)
{
    // GIVEN
    test::TestBase test{1, 2, 3};

    // WHEN
    static_assert(sizeof(test) > sizeof(2 * sizeof(int)));
    using BufferType = sample::detail::SmallBuffer<test::TestBase, 
        sizeof(test) - (2u * sizeof(int))>;
    BufferType buffer(std::in_place_type<decltype(test)>, test);
    BufferType buffer2(std::move(buffer));

    // THEN
    using namespace ::testing;
    ASSERT_THAT(*buffer2, Eq(test));
}

TEST(SmallBuffer, constructible_with_small_derived)
{
    // GIVEN
    test::TestDerived test{1, 2, 3, 4, 5, 6};

    // WHEN
    using Buffer = sample::detail::SmallBuffer<test::TestBase, sizeof(test)>;
    Buffer buffer(std::in_place_type<decltype(test)>, test);

    // THEN
    using namespace ::testing;
    ASSERT_THAT(dynamic_cast<test::TestDerived&>(*buffer), Eq(test));
}

TEST(SmallBuffer, constructible_with_large_derived)
{
    // GIVEN
    test::TestDerived test{1, 2, 3, 4, 5, 6};

    // WHEN
    static_assert(sizeof(test) > sizeof(2 * sizeof(int)));
    using Buffer = sample::detail::SmallBuffer<test::TestBase, 
        sizeof(test) - (2u * sizeof(int))>;
    Buffer buffer(std::in_place_type<decltype(test)>, test);

    // THEN
    using namespace ::testing;
    ASSERT_THAT(dynamic_cast<test::TestDerived&>(*buffer), Eq(test));
}

TEST(SmallBuffer, small_derived_copyable)
{
    // GIVEN
    test::TestDerived test{1, 2, 3, 4, 5, 6};

    // WHEN
    using Buffer = sample::detail::SmallBuffer<test::TestBase, sizeof(test)>;
    Buffer buffer(std::in_place_type<decltype(test)>, test);
    Buffer buffer2(buffer);

    // THEN
    using namespace ::testing;
    ASSERT_THAT(dynamic_cast<test::TestDerived&>(*buffer2), Eq(test));
}

TEST(SmallBuffer, small_derived_movable)
{
    // GIVEN
    test::TestDerived test{1, 2, 3, 4, 5, 6};

    // WHEN
    using Buffer = sample::detail::SmallBuffer<test::TestBase, sizeof(test)>;
    Buffer buffer(std::in_place_type<decltype(test)>, test);
    Buffer buffer2(std::move(buffer));

    // THEN
    using namespace ::testing;
    ASSERT_THAT(dynamic_cast<test::TestDerived&>(*buffer2), Eq(test));
}

TEST(SmallBuffer, large_derived_copyable)
{
    // GIVEN
    test::TestDerived test{1, 2, 3, 4, 5, 6};

    // WHEN
    static_assert(sizeof(test) > sizeof(2 * sizeof(int)));
    using BufferType = sample::detail::SmallBuffer<test::TestBase, 
        sizeof(test) - (2u * sizeof(int))>;
    BufferType buffer(std::in_place_type<decltype(test)>, test);
    BufferType buffer2(buffer);

    // THEN
    using namespace ::testing;
    ASSERT_THAT(dynamic_cast<test::TestDerived&>(*buffer2), Eq(test));
}

TEST(SmallBuffer, large_derived_movable)
{
    // GIVEN
    test::TestDerived test{1, 2, 3, 4, 5, 6};

    // WHEN
    static_assert(sizeof(test) > sizeof(2 * sizeof(int)));
    using BufferType = sample::detail::SmallBuffer<test::TestBase, 
        sizeof(test) - (2u * sizeof(int))>;
    BufferType buffer(std::in_place_type<decltype(test)>, test);
    BufferType buffer2(std::move(buffer));

    // THEN
    using namespace ::testing;
    ASSERT_THAT(dynamic_cast<test::TestDerived&>(*buffer2), Eq(test));
}
