#ifndef INCLUDED_SAMPLE_COMPATIBLEITERATORS
#define INCLUDED_SAMPLE_COMPATIBLEITERATORS

#include <type_traits>

namespace sample::detail {

template <typename Category1, typename Category2,
          typename ValueType1, typename ValueType2,
          typename Reference1, typename Reference2,
          typename Pointer1, typename Pointer2,
          typename DifferenceType1, typename DifferenceType2,
          typename = void>
struct compatible_iterators : std::false_type 
{};

template <typename Category1, typename Category2,
          typename ValueType1, typename ValueType2,
          typename Reference1, typename Reference2,
          typename Pointer1, typename Pointer2,
          typename DifferenceType1, typename DifferenceType2>
struct compatible_iterators<Category1, Category2,
    ValueType1, ValueType2, Reference1, Reference2, Pointer1,
    Pointer2, DifferenceType1, DifferenceType2,
    std::enable_if_t<
        std::is_base_of_v<Category1, Category2> &&
        std::is_same_v<ValueType1, ValueType2> &&
        std::is_same_v<Reference1, Reference2> &&
        std::is_same_v<Pointer1, Pointer2> &&
        std::is_same_v<DifferenceType1, DifferenceType2>
    >> : std::true_type 
{};

template <typename Category1, typename Category2,
          typename ValueType1, typename ValueType2,
          typename Reference1, typename Reference2,
          typename Pointer1, typename Pointer2,
          typename DifferenceType1, typename DifferenceType2>
inline constexpr bool compatible_iterators_v = compatible_iterators<Category1,
    Category2, ValueType1, ValueType2, Reference1, Reference2,
    Pointer1, Pointer2, DifferenceType1, DifferenceType2>::value;

} // close namespace sample::detail

#endif // INCLUDED_SAMPLE_COMPATIBLEITERATORS
