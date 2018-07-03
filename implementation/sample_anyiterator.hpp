#ifndef SAMPLE_ANYITERATOR
#define SAMPLE_ANYITERATOR

#include <sample_anyiterator_base.hpp>
#include <sample_anyinputiterator_base.hpp>
#include <sample_smallbuffer.hpp>
#include <sample_rangecheck.hpp>

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

    // ACCESSORS
    bool operator==(const any_iterator& rhs) const noexcept;
    bool operator!=(const any_iterator& rhs) const noexcept;

protected:
    // PRIVATE TYPES
    template <typename T>
    struct Key { using value_type = T; };

protected:
    // PRIVATE CREATORS
    template <typename It, typename Target>
    any_iterator(It&& it, Key<Target>);

protected:
    // PRIVATE ACCESSORS
    detail::AnyIterator_Base& base() const noexcept;

private:
    // DATA
    detail::SmallBuffer<detail::AnyIterator_Base> d_buffer;
};

// Specialization of `any_iterator` which models the `InputIterator`
// named concept.
template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
struct any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType> 
        : any_iterator<void, ValueType, ReferenceType, 
                       PointerType, DifferenceType>
{
    // TYPES
    using value_type = ValueType;
    using reference = ReferenceType;
    using pointer = PointerType;
    using difference_type = DifferenceType;
    using iterator_category = std::input_iterator_tag;

    // CREATORS
    template <typename IteratorCategory2,
              typename ValueType2, typename ReferenceType2,
              typename PointerType2, typename DifferenceType2,
              typename = std::enable_if_t<
                (std::is_base_of_v<std::input_iterator_tag, IteratorCategory2>
                    || std::is_same_v<std::input_iterator_tag, IteratorCategory2>)
                && std::is_convertible_v<ValueType2, ValueType>
                && std::is_convertible_v<ReferenceType2, ReferenceType>
                && std::is_convertible_v<PointerType2, PointerType>
                && detail::range_check<PointerType2, PointerType>()>>
    any_iterator(const any_iterator<IteratorCategory2, ValueType2, 
                                    ReferenceType2, PointerType2, 
                                    DifferenceType2>& other);
    template <typename InputIt>
    any_iterator(InputIt it);

    // ACCESSORS
    reference operator*() const;
    pointer operator->() const;

private:
    // PRIVATE TYPES
    using BaseClass = any_iterator<void, ValueType, ReferenceType, 
                                   PointerType, DifferenceType>;
};

template <typename ValueType, typename Reference = ValueType&,
          typename Pointer = ValueType*, 
          typename DifferenceType = std::ptrdiff_t>
using any_input_iterator = any_iterator<std::input_iterator_tag, ValueType, 
                                        Reference, Pointer, DifferenceType>;

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
template <typename IteratorCategory, typename ValueType, 
          typename ReferenceType, typename PointerType, 
          typename DifferenceType>
template <typename It, typename Target>
inline any_iterator<IteratorCategory, ValueType, ReferenceType, PointerType,
                    DifferenceType>::any_iterator(It&& it, Key<Target>)
    : d_buffer(Target(std::forward<It>(it)))
{}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
template <typename IteratorCategory2, typename ValueType2,
          typename ReferenceType2, typename PointerType2,
          typename DifferenceType2, typename>
inline any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator(
                        const any_iterator<IteratorCategory2, ValueType2,
                                           ReferenceType2, PointerType2,
                                           DifferenceType2>& other)
    : BaseClass(other)
{}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
template <typename InputIt>
inline any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator(InputIt it)
    : BaseClass(std::move(it), 
        typename BaseClass::template Key<detail::AnyInputIterator_Impl<InputIt>>{})
{}

// ACCESSORS
template <typename IteratorCategory, typename ValueType, 
          typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline bool any_iterator<IteratorCategory, ValueType, ReferenceType,
                         PointerType, DifferenceType>::operator==(
                             const any_iterator& rhs) const noexcept
{
    return *d_buffer == *rhs.d_buffer;
}

template <typename IteratorCategory, typename ValueType, 
          typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline bool any_iterator<IteratorCategory, ValueType, ReferenceType,
                         PointerType, DifferenceType>::operator!=(
                             const any_iterator& rhs) const noexcept
{
    return *d_buffer != *rhs.d_buffer;
}

template <typename IteratorCategory, typename ValueType, 
          typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline detail::AnyIterator_Base& any_iterator<IteratorCategory,
    ValueType, ReferenceType, PointerType, DifferenceType>::base() const
    noexcept
{
    return *d_buffer;
}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
inline typename any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::reference
    any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                 PointerType, DifferenceType>::operator*() const
{
    return *static_cast<detail::AnyInputIterator_Base<ValueType, 
        ReferenceType, PointerType>&>(BaseClass::base());
}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
inline typename any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::pointer
    any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                 PointerType, DifferenceType>::operator->() const
{
    return static_cast<detail::AnyInputIterator_Base<ValueType, 
        ReferenceType, PointerType>&>(BaseClass::base()).operator->();
}

// MANIPULATORS
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
