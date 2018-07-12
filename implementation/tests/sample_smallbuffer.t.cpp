#include <sample_smallbuffer.hpp>

#include <array>
#include <cassert>
#include <cstdlib>
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

    struct CountingResource : std::experimental::pmr::memory_resource
    {
        // TYPES
        using BaseResource = std::experimental::pmr::memory_resource;
        
        // ACCESSORS
        bool do_is_equal(const BaseResource& other) const noexcept override
        {
            return dynamic_cast<const CountingResource*>(&other);
        }

        // MANIPULATORS
        void* do_allocate(std::size_t bytes, std::size_t alignment) override
        {
            assert(alignment <= alignof(std::max_align_t));
            void* const ptr = ::operator new(bytes);
            d_numBytes += bytes;
            ++d_numAllocations;
            d_numOutstanding += bytes;
            ++d_numOutstandingBlocks;
            return ptr;
        }

        void do_deallocate(void* p, std::size_t bytes, std::size_t alignment) 
            override
        {
            assert(alignment <= alignof(std::max_align_t));
            ::operator delete(p, bytes);

            d_numOutstanding -= bytes;
            --d_numOutstandingBlocks;
        }

        // DATA
        std::atomic<std::size_t> d_numBytes{(std::size_t)0ul};
        std::atomic<std::size_t> d_numAllocations{(std::size_t)0ul};
        std::atomic<std::ptrdiff_t> d_numOutstanding{(std::ptrdiff_t)0l};
        std::atomic<std::ptrdiff_t> d_numOutstandingBlocks{(std::ptrdiff_t)0l};
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

TEST(SmallBuffer, doesnt_use_memory_resource_when_fit_in_buffer)
{
    // GIVEN
    test::TestBase test{1, 2, 3};
    test::CountingResource resource;

    // WHEN
    using Buffer = sample::detail::SmallBuffer<test::TestBase, sizeof(test)>;
    {
        Buffer buffer(std::allocator_arg, &resource,
            std::in_place_type<decltype(test)>, test);
    }

    // THEN
    using namespace ::testing;
    EXPECT_THAT(resource.d_numAllocations, Eq(0ul));
    EXPECT_THAT(resource.d_numBytes, Eq(0ul));
    EXPECT_THAT(resource.d_numOutstanding, Eq(0l));
    EXPECT_THAT(resource.d_numOutstandingBlocks, Eq(0l));
}

TEST(SmallBuffer, uses_memory_resource_when_doesnt_fit_in_buffer)
{
    // GIVEN
    test::TestBase test{1, 2, 3};
    test::CountingResource resource;

    // WHEN
    static_assert(sizeof(test) > sizeof(2 * sizeof(int)));
    using BufferType = sample::detail::SmallBuffer<test::TestBase, 
        sizeof(test) - (2u * sizeof(int))>;
    {
        BufferType buffer(std::allocator_arg, &resource,
            std::in_place_type<decltype(test)>, test);
    }

    // THEN
    using namespace ::testing;
    EXPECT_THAT(resource.d_numAllocations, Eq(1ul));
    EXPECT_THAT(resource.d_numBytes, Eq(sizeof(test)));
    EXPECT_THAT(resource.d_numOutstanding, Eq(0l));
    EXPECT_THAT(resource.d_numOutstandingBlocks, Eq(0l));
}
