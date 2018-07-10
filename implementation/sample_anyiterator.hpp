#ifndef SAMPLE_ANYITERATOR
#define SAMPLE_ANYITERATOR

#include <sample_anyiterator_base.hpp>
#include <sample_anyinputiterator_base.hpp>
#include <sample_anyoutputiterator_base.hpp>
#include <sample_anyforwarditerator_base.hpp>
#include <sample_anybidirectionaliterator_base.hpp>
#include <sample_anyrandomaccessiterator_base.hpp>
#include <sample_smallbuffer.hpp>
#include <sample_util.hpp>

#include <cstddef>
#include <iterator>
#include <type_traits>

namespace sample {
template <typename IteratorCategory, 
          typename ValueType,
          typename ReferenceType = ValueType&,
          typename PointerType = ValueType*,
          typename DifferenceType = std::ptrdiff_t>
struct any_iterator;

inline namespace {
    template <typename ValueType, 
            typename ReferenceType = ValueType&,
            typename PointerType = ValueType*, 
            typename DifferenceType = std::ptrdiff_t>
    using any_input_iterator = any_iterator<std::input_iterator_tag, ValueType,
        ReferenceType, PointerType, DifferenceType>;

    template <typename ValueType, 
            typename ReferenceType = ValueType&,
            typename PointerType = ValueType*, 
            typename DifferenceType = std::ptrdiff_t>
    using any_output_iterator = any_iterator<std::output_iterator_tag, ValueType,
        ReferenceType, PointerType, DifferenceType>;

    template <typename ValueType, 
            typename ReferenceType = ValueType&,
            typename PointerType = ValueType*, 
            typename DifferenceType = std::ptrdiff_t>
    using any_forward_iterator = any_iterator<std::forward_iterator_tag, ValueType,
        ReferenceType, PointerType, DifferenceType>;

    template <typename ValueType, 
            typename ReferenceType = ValueType&,
            typename PointerType = ValueType*, 
            typename DifferenceType = std::ptrdiff_t>
    using any_bidirectional_iterator = any_iterator<std::bidirectional_iterator_tag, 
        ValueType, ReferenceType, PointerType, DifferenceType>;

    template <typename ValueType, 
            typename ReferenceType = ValueType&,
            typename PointerType = ValueType*, 
            typename DifferenceType = std::ptrdiff_t>
    using any_random_access_iterator = any_iterator<std::random_access_iterator_tag, 
        ValueType, ReferenceType, PointerType, DifferenceType>;
} // close anonymous inline namespace

template <typename IteratorCategory, typename ValueType,
          typename ReferenceType, typename PointerType,
          typename DifferenceType>
struct any_iterator {
    // TYPES
    using value_type = ValueType;
    using reference = ReferenceType;
    using pointer = PointerType;
    using difference_type = DifferenceType;
    using iterator_category = IteratorCategory;

    // CREATORS
    template <bool True = true, 
              typename = std::enable_if_t<True && std::is_base_of_v<
                std::forward_iterator_tag,
                iterator_category
              >>>
    any_iterator() noexcept;
        // Default construct an `any_iterator` (thus constructing it in the
        // singular iterator state).
        //
        // Only participates in the overload set if `IteratorCategory` is
        // derived from `std::forward_iterator_tag`.

    any_iterator(const any_iterator&);
        // Copy construct an `any_iterator` from an identically specified
        // `any_iterator`.
        //
        // Throws if allocation was required and failed, or if the copy
        // constructor of `It` throws.

    any_iterator(any_iterator&&);
        // Move construct an `any_iterator` from an identically specified
        // `any_iterator`.
        //
        // Throws if allocation was required and failed, or if the move
        // constructor of `It` threw.

    template <typename It, 
              typename = std::enable_if_t<std::is_base_of_v<
                iterator_category, 
                typename std::iterator_traits<It>::iterator_category
              >>>
    any_iterator(It it);
        // Construct an `any_iterator` from an `It`.
        //
        // Only participates in the overload set if `It` satisfies the
        // IteratorCategory of this `any_iterator`.
        //
        // Throws if allocation was required and failed, or if the move
        // constructor of `It` throws.

    template <typename OtherAnyIterator,
              typename = std::enable_if_t<detail::is_compatible_iterator_v<
                any_iterator, OtherAnyIterator>>>
    any_iterator(OtherAnyIterator&& other_any_iterator);
        // Construct an `any_iterator` from the underlying iterator of 
        // `other_any_iterator`.
        //
        // Only participates in the overload set if `OtherAnyIterator` 
        // is a compatible iterator of this `any_iterator`, i.e. it 
        // is also an `any_iterator`, with the `iterator_category` 
        // derived from `iterator_category` and all other template
        // parameters the same.
        //
        // Throws if allocation was required and failed, or if the 
        // relevant constructor of the underlying iterator throws.

    ~any_iterator();
        // Destroys the `any_iterator`, releasing any memory that was allocated
        // and calling the destructor of the underlying iterator object.

    // ACCESSORS
    void* base() const noexcept;
        // Returns a pointer to the underlying iterator, or the null pointer if
        // the `any_iterator` was default constructed.

    template <bool True = true>
    std::enable_if_t<True && std::is_base_of_v<std::input_iterator_tag, 
        iterator_category>, reference> operator*() const;
        // Dereference the `any_iterator` yielding a `reference` produced from
        // dereferencing the underlying iterator that the `any_iterator` was
        // created from.
        //
        // Only participates in overload resolution if `iterator_category` is 
        // derived from `input_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterator
        // is not dereferencable.

    template <bool True = true, typename = std::enable_if_t<True && 
        std::is_base_of_v<std::input_iterator_tag, iterator_category>>>
    const detail::AnyInputIterator_Base<value_type, reference, pointer>& operator->() 
        const;
        // Returns an implementation-defined type which implements `operator->`
        // which returns `pointer`.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `input_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterator
        // is not dereferencable.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    reference operator[](difference_type offset) const;
        // Dereferences the iterator after advancing `offset` times, effectively
        // calls `underlying_iterator[offset]` and returns the result.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `random_access_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterator
        // is not dereferencable.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::input_iterator_tag, iterator_category>>>
    bool operator==(const any_iterator& rhs) const;
        // Compares two `any_iterators` for equality, by calling `operator==` on 
        // their underlying iterators
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `input_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterators
        // of `*this` and `rhs` are not of the same type, or if it is undefined
        // behaviour to compare them for equality.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::input_iterator_tag, iterator_category>>>
    bool operator!=(const any_iterator& rhs) const;
        // Compares two `any_iterators` for inequality, by calling `operator!=` on 
        // their underlying iterators
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `input_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterators
        // of `*this` and `rhs` are not of the same type, or if it is undefined
        // behaviour to compare them for inequality.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    bool operator<(const any_iterator& rhs) const;
        // Returns the result of whether the underlying iterator of `*this` is less
        // than the underlying iterator of `rhs`, by calling `operator<` on
        // their underlying iterators.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `random_access_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterators
        // of `*this` and `rhs` are not of the same type, or if it is undefined
        // behaviour to compare them with `operator<`.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    bool operator>(const any_iterator& rhs) const;
        // Returns the result of whether the underlying iterator of `*this` is 
        // greater than the underlying iterator of `rhs`, by calling `operator>` 
        // on their underlying iterators.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `random_access_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterators
        // of `*this` and `rhs` are not of the same type, or if it is undefined
        // behaviour to compare them with `operator>`.
    
    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    bool operator<=(const any_iterator& rhs) const;
        // Returns the result of whether the underlying iterator of `*this` is 
        // less than or equal to the underlying iterator of `rhs`, by calling 
        // `operator<=` on their underlying iterators.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `random_access_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterators
        // of `*this` and `rhs` are not of the same type, or if it is undefined
        // behaviour to compare them with `operator<=`.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    bool operator>=(const any_iterator& rhs) const;
        // Returns the result of whether the underlying iterator of `*this` is 
        // greater than or equal to the underlying iterator of `rhs`, by calling 
        // `operator>=` on their underlying iterators.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `random_access_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterators
        // of `*this` and `rhs` are not of the same type, or if it is undefined
        // behaviour to compare them with `operator>=`.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    any_iterator operator+(difference_type offset) const;
        // Returns a copy of the `any_iterator` where the underlying iterator
        // has been advanced by `offset`.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `random_access_iterator_tag`.
        //
        // The behaviour of this function is undefined unless it is well-defined
        // to call `it += offset`, where `it` is the underlying iterator of 
        // `*this`.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    any_iterator operator-(difference_type offset) const;
        // Returns a copy of the `any_iterator` where the underlying iterator
        // has been retreated by `offset`.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `random_access_iterator_tag`.
        //
        // The behaviour of this function is undefined unless it is well-defined
        // to call `it -= offset`, where `it` is the underlying iterator of 
        // `*this`.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    difference_type operator-(const any_iterator& rhs) const;
        // Returns the distance between `rhs` and `*this`, as computed as if by 
        // calling: `this->underlying_iterator - rhs.underlying_iterator`.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `random_access_iterator_tag`.
        //
        // The behaviour of this function is undefined if `*this` and `rhs` do
        // not have underlying iterators of the same type, or if it is undefined
        // behaviour to subtract `rhs.underlying_iterator` from 
        // `this->underlying_iterator`.

    // MANIPULATORS
    void swap(any_iterator& other) noexcept;
        // Swaps the underlying iterators of `*this` and `other`. 

    any_iterator& operator++();
        // Increments the underlying iterator contained within this `any_iterator`
        // and returns a reference to `*this`.
        //
        // The behaviour of this function is undefined if the underlying iterator
        // is not incrementable.

    template <bool True = true>
    std::enable_if_t<True && std::is_base_of_v<std::output_iterator_tag, 
        iterator_category>, any_iterator&> operator*();
        // Performs a `no-op` on the `any_iterator`, returning a reference to *this.
        //
        // Only participates in overload resolution if `iterator_category` is 
        // derived from `output_iterator_tag`.

    template <bool True = true>
    std::enable_if_t<True && std::is_base_of_v<std::output_iterator_tag, 
        iterator_category>, any_iterator&> operator=(value_type value);
        // Assigns `value` to the underlying iterator.
        //
        // Only participates in overload resolution if `iterator_category` is 
        // derived from `output_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterator
        // is invalid.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::forward_iterator_tag, iterator_category>>>
    any_iterator operator++(int);
        // Creates a copy of this `any_iterator` and then increments `*this`,
        // then returns the copy.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `forward_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterator
        // is not incrementable.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>>>
    any_iterator& operator--();
        // Decrements the underlying iterator contained within this `any_iterator`
        // and returns a reference to `*this`.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `bidirectional_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterator
        // is not decrementable.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::bidirectional_iterator_tag, iterator_category>>>
    any_iterator operator--(int);
        // Creates a copy of this `any_iterator` and then decrements `*this`,
        // then returns the copy.
        //
        // Only participates in overload resolution if the `iterator_category` is
        // derived from `bidirectional_iterator_tag`.
        //
        // The behaviour of this function is undefined if the underlying iterator
        // is not decrementable.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    any_iterator& operator+=(difference_type offset);
        // Advances the underlying iterator of `*this` by `offset`, then 
        // returns a reference to `*this`.
        //
        // Only participates in overload resolution if `iterator_category` is
        // derived from `random_access_iterator_tag`.
        // 
        // The behaviour of this function is undefined if the underlying iterator
        // cannot be advanced by `offset`.

    template <bool True = true, typename = std::enable_if_t<True &&
        std::is_base_of_v<std::random_access_iterator_tag, iterator_category>>>
    any_iterator& operator-=(difference_type offset);
        // Retreats the underlying iterator of `*this` by `offset`, then 
        // returns a reference to `*this`.
        //
        // Only participates in overload resolution if `iterator_category` is
        // derived from `random_access_iterator_tag`.
        // 
        // The behaviour of this function is undefined if the underlying iterator
        // cannot be advanced by `offset`.

private:
    // FRIENDS
    template <typename OtherCategory, typename OtherValue,
        typename OtherReference, typename OtherPointer,
        typename OtherDifferenceType>
    friend class any_iterator;

private:
    // PRIVATE TYPES
    using BufferType = detail::SmallBuffer<detail::AnyIterator_Base>;

private:
    // PRIVATE CREATORS
    any_iterator(const std::random_access_iterator_tag&) noexcept;
    any_iterator(const std::bidirectional_iterator_tag&) noexcept;
    any_iterator(const std::forward_iterator_tag&) noexcept;

    template <typename RandIt>
    any_iterator(const std::random_access_iterator_tag&, RandIt&& it);
    template <typename BiDirIt>
    any_iterator(const std::bidirectional_iterator_tag&, BiDirIt&& it);
    template <typename FwdIt>
    any_iterator(const std::forward_iterator_tag&, FwdIt&& it);
    template <typename InIt>
    any_iterator(const std::input_iterator_tag&, InIt&& it);
    template <typename OutIt>
    any_iterator(const std::output_iterator_tag&, OutIt&& it);

private:
    // DATA
    BufferType d_buffer;
};

template <typename It>
any_iterator(It) -> any_iterator<
    typename std::iterator_traits<It>::iterator_category,
    typename std::iterator_traits<It>::value_type,
    typename std::iterator_traits<It>::reference,
    typename std::iterator_traits<It>::pointer,
    typename std::iterator_traits<It>::difference_type
>;

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
void swap(any_iterator<IteratorCategory, ValueType, Reference, 
    Pointer, DifferenceType>& lhs,
          any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType>& rhs) noexcept;
    // Swaps the underlying iterators of `lhs` and `rhs`.

// FREE OPERATORS
template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType,
          typename = std::enable_if_t<
            std::is_base_of_v<std::random_access_iterator_tag, IteratorCategory>>>
any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType> operator+(
        typename any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
            DifferenceType>::difference_type offset, 
        const any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
            DifferenceType>& rhs);
    // Returns a copy of `rhs` and advances its underlying iterator by
    // `offset`.
    //
    // Only participates in overload resolution if `IteratorCategory` is 
    // derived from `random_access_iterator_tag`.
    //
    // The behaviour of this function is undefined unless the underlying
    // iterator of `rhs` can be advanced by `offset`.

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool True, typename>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator() noexcept
    : any_iterator(IteratorCategory{})
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const any_iterator&) = default;

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(any_iterator&&) = default;

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <typename It, typename>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(It it)
    : any_iterator(IteratorCategory{}, std::move(it))
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <typename OtherAnyIterator, typename>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(OtherAnyIterator&& other_any_iterator)
    : any_iterator(detail::forward_like<OtherAnyIterator>(
        other_any_iterator.d_buffer))
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::~any_iterator() = default;

// ACCESSORS
template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline void* any_iterator<IteratorCategory, ValueType,
        Reference, Pointer, DifferenceType>::base() const noexcept
{
    return d_buffer->base();
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool True>
inline std::enable_if_t<True && std::is_base_of_v<std::input_iterator_tag, 
    IteratorCategory>, typename any_iterator<IteratorCategory, ValueType,
        Reference, Pointer, DifferenceType>::reference> 
    any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
        DifferenceType>::operator*() const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    using InputType = detail::AnyInputIterator_Base<ValueType, Reference, 
        Pointer>;
    
    assert(dynamic_cast<InputType*>(&underlying));
    return *static_cast<InputType&>(underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline const detail::AnyInputIterator_Base<ValueType, Reference, Pointer>& 
    any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
        DifferenceType>::operator->() const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    using InputType = detail::AnyInputIterator_Base<ValueType, Reference, 
        Pointer>;
    
    assert(dynamic_cast<InputType*>(&underlying));
    return static_cast<InputType&>(underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline Reference any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType>::operator[](difference_type offset) const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    using InputType = detail::AnyRandomAccessIterator_Base<ValueType, 
        Reference, Pointer, DifferenceType>;
    
    assert(dynamic_cast<InputType*>(&underlying));
    return static_cast<InputType&>(underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline bool any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType>::operator==(const any_iterator& rhs) const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    detail::AnyIterator_Base& rhs_underlying = *rhs.d_buffer;

    using RequiredType = detail::AnyInputIterator_Base<ValueType, Reference, 
        Pointer>;

    assert(dynamic_cast<RequiredType*>(&underlying));
    assert(dynamic_cast<RequiredType*>(&rhs_underlying));
    return static_cast<RequiredType&>(underlying) == 
        static_cast<RequiredType&>(rhs_underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline bool any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType>::operator!=(const any_iterator& rhs) const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    detail::AnyIterator_Base& rhs_underlying = *rhs.d_buffer;

    using RequiredType = detail::AnyInputIterator_Base<ValueType, Reference, 
        Pointer>;

    assert(dynamic_cast<RequiredType*>(&underlying));
    assert(dynamic_cast<RequiredType*>(&rhs_underlying));
    return static_cast<RequiredType&>(underlying) != 
        static_cast<RequiredType&>(rhs_underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline bool any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType>::operator<(const any_iterator& rhs) const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    detail::AnyIterator_Base& rhs_underlying = *rhs.d_buffer;

    using RequiredType = detail::AnyRandomAccessIterator_Base<ValueType, 
        Reference, Pointer, DifferenceType>;

    assert(dynamic_cast<RequiredType*>(&underlying));
    assert(dynamic_cast<RequiredType*>(&rhs_underlying));
    return static_cast<RequiredType&>(underlying) < 
        static_cast<RequiredType&>(rhs_underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline bool any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType>::operator>(const any_iterator& rhs) const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    detail::AnyIterator_Base& rhs_underlying = *rhs.d_buffer;

    using RequiredType = detail::AnyRandomAccessIterator_Base<ValueType, 
        Reference, Pointer, DifferenceType>;

    assert(dynamic_cast<RequiredType*>(&underlying));
    assert(dynamic_cast<RequiredType*>(&rhs_underlying));
    return static_cast<RequiredType&>(underlying) > 
        static_cast<RequiredType&>(rhs_underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline bool any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType>::operator<=(const any_iterator& rhs) const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    detail::AnyIterator_Base& rhs_underlying = *rhs.d_buffer;

    using RequiredType = detail::AnyRandomAccessIterator_Base<ValueType, 
        Reference, Pointer, DifferenceType>;

    assert(dynamic_cast<RequiredType*>(&underlying));
    assert(dynamic_cast<RequiredType*>(&rhs_underlying));
    return static_cast<RequiredType&>(underlying) <=
        static_cast<RequiredType&>(rhs_underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline bool any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType>::operator>=(const any_iterator& rhs) const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    detail::AnyIterator_Base& rhs_underlying = *rhs.d_buffer;

    using RequiredType = detail::AnyRandomAccessIterator_Base<ValueType, 
        Reference, Pointer, DifferenceType>;

    assert(dynamic_cast<RequiredType*>(&underlying));
    assert(dynamic_cast<RequiredType*>(&rhs_underlying));
    return static_cast<RequiredType&>(underlying) >=
        static_cast<RequiredType&>(rhs_underlying);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType> any_iterator<IteratorCategory, ValueType, Reference, 
        Pointer, DifferenceType>::operator+(difference_type offset) const
{
    auto tmp{*this};
    return tmp += offset;
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType> any_iterator<IteratorCategory, ValueType, Reference, 
        Pointer, DifferenceType>::operator-(difference_type offset) const
{
    auto tmp{*this};
    return tmp -= offset;
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline DifferenceType any_iterator<IteratorCategory, ValueType, Reference, Pointer,
        DifferenceType>::operator-(const any_iterator& rhs) const
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    detail::AnyIterator_Base& rhs_underlying = *rhs.d_buffer;

    using RequiredType = detail::AnyRandomAccessIterator_Base<ValueType, 
        Reference, Pointer, DifferenceType>;

    assert(dynamic_cast<RequiredType*>(&underlying));
    assert(dynamic_cast<RequiredType*>(&rhs_underlying));
    return static_cast<RequiredType&>(underlying) -
        static_cast<RequiredType&>(rhs_underlying);
}

// MANIPULATORS
template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline void any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType>::swap(any_iterator& other) noexcept
{
    using std::swap;
    swap(d_buffer, other.d_buffer);
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType>& any_iterator<IteratorCategory, ValueType, 
    Reference, Pointer, DifferenceType>::operator++()
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    ++underlying;
    return *this;
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool True>
inline std::enable_if_t<True && std::is_base_of_v<std::output_iterator_tag, 
    IteratorCategory>, any_iterator<IteratorCategory, ValueType,
        Reference, Pointer, DifferenceType>&> 
    any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
        DifferenceType>::operator*()
{
#ifndef NDEBUG
    detail::AnyIterator_Base& underlying = *d_buffer;
    using OutputType = detail::AnyOutputIterator_Base<ValueType>;
    assert(dynamic_cast<OutputType*>(&underlying));
#endif // NDEBUG

    return *this;
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool True>
inline std::enable_if_t<True && std::is_base_of_v<std::output_iterator_tag, 
    IteratorCategory>, any_iterator<IteratorCategory, ValueType,
        Reference, Pointer, DifferenceType>&> 
    any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
        DifferenceType>::operator=(value_type value)
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    using OutputType = detail::AnyOutputIterator_Base<ValueType>;

    assert(dynamic_cast<OutputType*>(&underlying));
    static_cast<OutputType&>(underlying) = std::move(value);
    return *this;
}


template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType> any_iterator<IteratorCategory, ValueType, 
    Reference, Pointer, DifferenceType>::operator++(int)
{
    auto tmp{*this};
    ++*this;
    return tmp;
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType>& any_iterator<IteratorCategory, ValueType, 
    Reference, Pointer, DifferenceType>::operator--()
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    using RequiredType = detail::AnyBidirectionalIterator_Base<ValueType,
        Reference, Pointer>;
    
    assert(dynamic_cast<RequiredType*>(&underlying));
    --static_cast<RequiredType&>(underlying);
    return *this;
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType> any_iterator<IteratorCategory, ValueType, 
    Reference, Pointer, DifferenceType>::operator--(int)
{
    auto tmp{*this};
    --*this;
    return tmp;
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType>& any_iterator<IteratorCategory, ValueType, 
    Reference, Pointer, DifferenceType>::operator+=(difference_type offset)
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    using RequiredType = detail::AnyRandomAccessIterator_Base<ValueType,
        Reference, Pointer, DifferenceType>;
    
    assert(dynamic_cast<RequiredType*>(&underlying));
    static_cast<RequiredType&>(underlying) += offset;
    return *this;
}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <bool, typename>
inline any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType>& any_iterator<IteratorCategory, ValueType, 
    Reference, Pointer, DifferenceType>::operator-=(difference_type offset)
{
    detail::AnyIterator_Base& underlying = *d_buffer;
    using RequiredType = detail::AnyRandomAccessIterator_Base<ValueType,
        Reference, Pointer, DifferenceType>;
    
    assert(dynamic_cast<RequiredType*>(&underlying));
    static_cast<RequiredType&>(underlying) -= offset;
    return *this;
}

// PRIVATE CREATORS
template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const std::random_access_iterator_tag&) 
        noexcept
    : d_buffer(std::in_place_type<detail::AnyRandomAccessIterator_Impl<void, ValueType,
        Reference, Pointer, DifferenceType>>)
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const std::bidirectional_iterator_tag&) 
        noexcept
    : d_buffer(std::in_place_type<detail::AnyBidirectionalIterator_Impl<void, ValueType,
        Reference, Pointer>>)
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const std::forward_iterator_tag&) 
        noexcept
    : d_buffer(std::in_place_type<detail::AnyForwardIterator_Impl<void, ValueType,
        Reference, Pointer>>)
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <typename RandIt>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const std::random_access_iterator_tag&,
        RandIt&& it) 
    : d_buffer(std::in_place_type<detail::AnyRandomAccessIterator_Impl<std::decay_t<RandIt>, 
        ValueType, Reference, Pointer, DifferenceType>>, std::forward<RandIt>(it))
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <typename BiDirIt>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const std::bidirectional_iterator_tag&,
        BiDirIt&& it) 
    : d_buffer(std::in_place_type<detail::AnyBidirectionalIterator_Impl<std::decay_t<BiDirIt>, 
        ValueType, Reference, Pointer>>, std::forward<BiDirIt>(it))
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <typename FwdIt>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const std::forward_iterator_tag&,
        FwdIt&& it) 
    : d_buffer(std::in_place_type<detail::AnyForwardIterator_Impl<std::decay_t<FwdIt>, 
        ValueType, Reference, Pointer>>, std::forward<FwdIt>(it))
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <typename InIt>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const std::input_iterator_tag&,
        InIt&& it) 
    : d_buffer(std::in_place_type<detail::AnyInputIterator_Impl<std::decay_t<InIt>, 
        ValueType, Reference, Pointer>>, std::forward<InIt>(it))
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
template <typename OutIt>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType>::any_iterator(const std::output_iterator_tag&,
        OutIt&& it) 
    : d_buffer(std::in_place_type<detail::AnyOutputIterator_Impl<std::decay_t<OutIt>, 
        ValueType>>, std::forward<OutIt>(it))
{}

template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType>
inline void swap(any_iterator<IteratorCategory, ValueType, Reference, 
    Pointer, DifferenceType>& lhs,
          any_iterator<IteratorCategory, ValueType, Reference,
    Pointer, DifferenceType>& rhs) noexcept
{
    lhs.swap(rhs);
}

// FREE OPERATORS
template <typename IteratorCategory, typename ValueType,
          typename Reference, typename Pointer, typename DifferenceType,
          typename>
inline any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
    DifferenceType> operator+(
        typename any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
            DifferenceType>::difference_type offset, 
        const any_iterator<IteratorCategory, ValueType, Reference, Pointer, 
            DifferenceType>& rhs)
{
    return rhs + offset;
}

} // close namespace sample

#endif // SAMPLE_ANYITERATOR
