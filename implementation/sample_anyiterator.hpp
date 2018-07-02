#ifndef SAMPLE_ANYITERATOR
#define SAMPLE_ANYITERATOR

#include <sample_anyiterator_base.hpp>
#include <sample_smallbuffer.hpp>

namespace sample {

template <typename IteratorCategory, 
          typename ValueType,
          typename ReferenceType = ValueType&,
          typename PointerType = ValueType*,
          typename DifferenceType = std::ptrdiff_t>
struct any_iterator {
    // TYPES
    using value_type = ValueType;
    using reference = ReferenceType;
    using pointer = PointerType;
    using difference_type = DifferenceType;
    using iterator_category = IteratorCategory;

    // CREATORS
    any_iterator() noexcept = default;
    any_iterator(const any_iterator&) = default;
    any_iterator(any_iterator&&) = default;

    // MANIPULATORS
    any_iterator& operator=(const any_iterator& rhs) = default;
    any_iterator& operator++();

private:
    // DATA
    detail::SmallBuffer<detail::AnyIterator_Base> d_buffer;
};

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
struct any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType> 
        : any_iterator<void, ValueType, ReferenceType, 
                       PointerType, DifferenceType>
{
    // TYPES
    using iterator_category = std::input_iterator_tag;

    // CREATORS
    template <typename IteratorCategory2,
              typename ValueType2, typename ReferenceType2,
              typename PointerType2, typename DifferenceType2,
              typename = std::enable_if_t<
                std::is_base_of_v<std::input_iterator_tag, IteratorCategory2>
                && std::is_convertible_v<ValueType2, ValueType>
                && std::is_convertible_v<ReferenceType2, ReferenceType>
                && std::is_convertible_v<PointerType2, PointerType>
                && detail::range_check<PointerType2, PointerType>()>
    any_iterator(const any_iterator<IteratorCategory2>&);
    template <typename InputIt>
    any_iterator(InputIt it);

    // ACCESSORS
    reference operator*() const;
    pointer operator->() const;

    // MANIPULATORS
    any_iterator operator++(int); 
        // TODO: Ascertain whether this should return some sort of proxy
        // object such that `*it++` works and is convertible to `value_type`
        // without having to return a copy of `any_iterator`.
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
template <typename IteratorCategory, typename ValueType, 
          typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline any_iterator<IteratorCategory, ValueType, ReferenceType, PointerType,
                    DifferenceType>& any_iterator<IteratorCategory, ValueType, 
                    ReferenceType, PointerType, DifferenceType>::operator++()
{
    this->d_buffer->operator++();
    return *this;
}


} // close namespace sample

#endif // SAMPLE_ANYITERATOR
