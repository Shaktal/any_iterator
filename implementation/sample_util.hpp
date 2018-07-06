#ifndef INCLUDED_SAMPLE_UTIL
#define INCLUDED_SAMPLE_UTIL

#include <limits>
#include <type_traits>

namespace sample::detail {

template <typename Iterator1, typename Iterator2,
          typename = void>
struct is_compatible_iterator : std::false_type {};

template <template <typename...> class IteratorType,
          typename Category1, typename Category2,
          typename ValueType, typename Reference,
          typename Pointer, typename DifferenceType>
struct is_compatible_iterator<
    IteratorType<Category1, ValueType, Reference,
        Pointer, DifferenceType>,
    IteratorType<Category2, ValueType, Reference,
        Pointer, DifferenceType>,
    std::enable_if_t<
        std::is_base_of_v<Category1, Category2>
    >
> : std::true_type {};

template <typename Iterator1, typename Iterator2>
constexpr bool is_compatible_iterator_v =
    is_compatible_iterator<Iterator1, Iterator2>::value;

template <class T, class U> 
  using apply_value_category_t = 
    std::conditional_t<std::is_lvalue_reference_v<T>,
                       std::remove_reference_t<U>&,
                       std::remove_reference_t<U>&&>;

template <typename Like, typename T>
constexpr apply_value_category_t<Like, T> forward_like(T&& value) noexcept;

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
template <typename Like, typename T>
inline constexpr apply_value_category_t<Like, T> forward_like(T&& value) 
    noexcept
{
    return static_cast<apply_value_category_t<Like, T>>(value);
}


} // close namespace sample::detail

#endif // INCLUDED_SAMPLE_UTIL
