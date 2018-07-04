#ifndef SAMPLE_ANYITERATOR
#define SAMPLE_ANYITERATOR

#include <sample_anyiterator_base.hpp>
#include <sample_anyinputiterator_base.hpp>
#include <sample_anyoutputiterator_base.hpp>
#include <sample_anyforwarditerator_base.hpp>
#include <sample_anybidirectionaliterator_base.hpp>
#include <sample_smallbuffer.hpp>
#include <sample_rangecheck.hpp>

#include <cassert>

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
    any_iterator(const any_iterator&) = default;
    any_iterator(any_iterator&&) = default;

    // MANIPULATORS
    any_iterator& operator=(const any_iterator& rhs) = default;
    any_iterator& operator++();

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

    bool operator==(const any_iterator& rhs) const noexcept;
    bool operator!=(const any_iterator& rhs) const noexcept;

private:
    // PRIVATE TYPES
    using BaseClass = any_iterator<void, ValueType, ReferenceType, 
                                   PointerType, DifferenceType>;

protected:
    // PRIVATE TYPES
    template <typename T>
    using Key = typename BaseClass::template Key<T>;

protected:
    // PRIVATE CREATORS
    using BaseClass::BaseClass;
};

template <typename ValueType, typename Reference = ValueType&,
          typename Pointer = ValueType*, 
          typename DifferenceType = std::ptrdiff_t>
using any_input_iterator = any_iterator<std::input_iterator_tag, ValueType, 
                                        Reference, Pointer, DifferenceType>;

// Specialization of `any_iterator` which models the `OutputIterator`
// named concept.
template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
struct any_iterator<std::output_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType> 
        : any_iterator<void, ValueType, ReferenceType, 
                       PointerType, DifferenceType>
{
    // TYPES
    using value_type = ValueType;
    using reference = ReferenceType;
    using pointer = PointerType;
    using difference_type = DifferenceType;
    using iterator_category = std::output_iterator_tag;

    // CREATORS
    template <typename IteratorCategory2,
              typename ValueType2, typename ReferenceType2,
              typename PointerType2, typename DifferenceType2,
              typename = std::enable_if_t<
                std::is_base_of_v<std::output_iterator_tag, IteratorCategory2>
                && std::is_convertible_v<ValueType2, ValueType>
                && std::is_convertible_v<ReferenceType2, ReferenceType>
                && std::is_convertible_v<PointerType2, PointerType>
                && detail::range_check<PointerType2, PointerType>()>>
    any_iterator(const any_iterator<IteratorCategory2, ValueType2, 
                                    ReferenceType2, PointerType2, 
                                    DifferenceType2>& other);
    template <typename OutputIt>
    any_iterator(OutputIt it);

    // ACCESSORS
    any_iterator& operator*() noexcept;
    const any_iterator& operator*() const noexcept;

    // MANIPULATORS
    template <typename U>
    any_iterator& operator=(U&& value);

private:
    // PRIVATE TYPES
    using BaseClass = any_iterator<void, ValueType, ReferenceType, 
                                   PointerType, DifferenceType>;
};

template <typename ValueType, typename Reference = ValueType&,
          typename Pointer = ValueType*, 
          typename DifferenceType = std::ptrdiff_t>
using any_output_iterator = any_iterator<std::output_iterator_tag, ValueType, 
                                        Reference, Pointer, DifferenceType>;

// Specialization of `any_iterator` which models the `ForwardIterator`
// named concept.
template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
struct any_iterator<std::forward_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType> 
        : any_iterator<std::input_iterator_tag, ValueType, ReferenceType, 
                       PointerType, DifferenceType>
{
    // TYPES
    using value_type = ValueType;
    using reference = ReferenceType;
    using pointer = PointerType;
    using difference_type = DifferenceType;
    using iterator_category = std::forward_iterator_tag;

private:
    // PRIVATE TYPES
    using BaseClass = any_iterator<std::input_iterator_tag, ValueType, ReferenceType, 
                                   PointerType, DifferenceType>;

public:
    // CREATORS
    any_iterator();

    template <typename IteratorCategory2,
              typename ValueType2, typename ReferenceType2,
              typename PointerType2, typename DifferenceType2,
              typename = std::enable_if_t<
                std::is_base_of_v<std::forward_iterator_tag, IteratorCategory2>
                && std::is_convertible_v<ValueType2, ValueType>
                && std::is_convertible_v<ReferenceType2, ReferenceType>
                && std::is_convertible_v<PointerType2, PointerType>
                && detail::range_check<PointerType2, PointerType>()>>
    any_iterator(const any_iterator<IteratorCategory2, ValueType2, 
                                    ReferenceType2, PointerType2, 
                                    DifferenceType2>& other);
    template <typename FwdIt>
    any_iterator(FwdIt it);

    // MANIPULATORS
    using BaseClass::operator++;
    any_iterator operator++(int);

protected:
    // PRIVATE TYPES
    template <typename T>
    using Key = typename BaseClass::template Key<T>;

protected:
    // PRIVATE CREATORS
    using BaseClass::BaseClass;

protected:
    // PRIVATE ACCESSORS
    using BaseClass::base;
};

template <typename ValueType, typename Reference = ValueType&,
          typename Pointer = ValueType*, 
          typename DifferenceType = std::ptrdiff_t>
using any_forward_iterator = any_iterator<std::forward_iterator_tag, ValueType, 
                                        Reference, Pointer, DifferenceType>;

// Specialization of `any_iterator` which models the `BidirectionalIterator`
// named concept.
template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
struct any_iterator<std::bidirectional_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType> 
        : any_iterator<std::forward_iterator_tag, ValueType, ReferenceType, 
                       PointerType, DifferenceType>
{
    // TYPES
    using value_type = ValueType;
    using reference = ReferenceType;
    using pointer = PointerType;
    using difference_type = DifferenceType;
    using iterator_category = std::bidirectional_iterator_tag;

    // CREATORS
    any_iterator();

    template <typename IteratorCategory2,
              typename ValueType2, typename ReferenceType2,
              typename PointerType2, typename DifferenceType2,
              typename = std::enable_if_t<
                std::is_base_of_v<std::bidirectional_iterator_tag, IteratorCategory2>
                && std::is_convertible_v<ValueType2, ValueType>
                && std::is_convertible_v<ReferenceType2, ReferenceType>
                && std::is_convertible_v<PointerType2, PointerType>
                && detail::range_check<PointerType2, PointerType>()>>
    any_iterator(const any_iterator<IteratorCategory2, ValueType2, 
                                    ReferenceType2, PointerType2, 
                                    DifferenceType2>& other);
    template <typename BiDirIt>
    any_iterator(BiDirIt it);

    // MANIPULATORS
    any_iterator& operator--();
    any_iterator  operator--(int);

private:
    // PRIVATE TYPES
    using BaseClass = any_iterator<std::forward_iterator_tag, ValueType, ReferenceType, 
                                   PointerType, DifferenceType>;

protected:
    // PRIVATE ACCESSORS
    using BaseClass::base;
};

template <typename ValueType, typename Reference = ValueType&,
          typename Pointer = ValueType*, 
          typename DifferenceType = std::ptrdiff_t>
using any_bidirectional_iterator = any_iterator<std::bidirectional_iterator_tag, ValueType, 
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

// InputIterator specializations
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
        typename BaseClass::template Key<detail::AnyInputIterator_Impl<InputIt, 
            ValueType, ReferenceType, PointerType>>{})
{}

// OutputIterator specializations
template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
template <typename IteratorCategory2, typename ValueType2,
          typename ReferenceType2, typename PointerType2,
          typename DifferenceType2, typename>
inline any_iterator<std::output_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator(
                        const any_iterator<IteratorCategory2, ValueType2,
                                           ReferenceType2, PointerType2,
                                           DifferenceType2>& other)
    : BaseClass(other)
{}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
template <typename OutputIt>
inline any_iterator<std::output_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator(OutputIt it)
    : BaseClass(std::move(it), 
        typename BaseClass::template Key<detail::AnyOutputIterator_Impl<OutputIt, 
            ValueType>>{})
{}

// ForwardIterator specializations
template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
inline any_iterator<std::forward_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator()
    : BaseClass(
        typename detail::AnyForwardIterator_Impl<void, ValueType, ReferenceType, 
            PointerType>::Key{},
        typename BaseClass::template Key<detail::AnyForwardIterator_Impl<void, 
            ValueType, ReferenceType, PointerType>>{})
{}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
template <typename IteratorCategory2, typename ValueType2,
          typename ReferenceType2, typename PointerType2,
          typename DifferenceType2, typename>
inline any_iterator<std::forward_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator(
                        const any_iterator<IteratorCategory2, ValueType2,
                                           ReferenceType2, PointerType2,
                                           DifferenceType2>& other)
    : BaseClass(other)
{}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
template <typename FwdIt>
inline any_iterator<std::forward_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator(FwdIt it)
    : BaseClass(std::move(it), 
        typename BaseClass::template Key<detail::AnyForwardIterator_Impl<FwdIt, 
            ValueType, ReferenceType, PointerType>>{})
{}

// BidirectionalIterator specializations
template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
inline any_iterator<std::bidirectional_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator()
    : BaseClass(
        typename detail::AnyForwardIterator_Impl<void, ValueType, ReferenceType, 
            PointerType>::Key{},
        typename BaseClass::template Key<detail::AnyForwardIterator_Impl<void, 
            ValueType, ReferenceType, PointerType>>{})
{}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
template <typename IteratorCategory2, typename ValueType2,
          typename ReferenceType2, typename PointerType2,
          typename DifferenceType2, typename>
inline any_iterator<std::bidirectional_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator(
                        const any_iterator<IteratorCategory2, ValueType2,
                                           ReferenceType2, PointerType2,
                                           DifferenceType2>& other)
    : BaseClass(other)
{}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
template <typename BiDirIt>
inline any_iterator<std::bidirectional_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::any_iterator(BiDirIt it)
    : BaseClass(std::move(it), 
        typename BaseClass::template Key<detail::AnyBidirectionalIterator_Impl<BiDirIt, 
            ValueType, ReferenceType, PointerType>>{})
{}

// ACCESSORS
template <typename ValueType, typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline bool any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                         PointerType, DifferenceType>::operator==(
                             const any_iterator& rhs) const noexcept
{
    detail::AnyIterator_Base& base = BaseClass::base();
    using InputType = detail::AnyInputIterator_Base<ValueType, 
        ReferenceType, PointerType>;
    assert((dynamic_cast<InputType*>(&base)));

    return static_cast<InputType&>(base) == 
        static_cast<InputType&>(rhs.base());
}

template <typename ValueType, typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline bool any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                         PointerType, DifferenceType>::operator!=(
                             const any_iterator& rhs) const noexcept
{
    detail::AnyIterator_Base& base = BaseClass::base();
    using InputType = detail::AnyInputIterator_Base<ValueType, 
        ReferenceType, PointerType>;
    assert((dynamic_cast<InputType*>(&base)));

    return static_cast<InputType&>(base) 
        != static_cast<InputType&>(rhs.base());
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
    detail::AnyIterator_Base& base = BaseClass::base();
    assert((dynamic_cast<detail::AnyInputIterator_Base<ValueType, 
        ReferenceType, PointerType>*>(&base)));

    return *static_cast<detail::AnyInputIterator_Base<ValueType, 
        ReferenceType, PointerType>&>(base);
}

template <typename ValueType, typename ReferenceType,
          typename PointerType, typename DifferenceType>
inline typename any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                    PointerType, DifferenceType>::pointer
    any_iterator<std::input_iterator_tag, ValueType, ReferenceType,
                 PointerType, DifferenceType>::operator->() const
{
    detail::AnyIterator_Base& base = BaseClass::base();
    assert((dynamic_cast<detail::AnyInputIterator_Base<ValueType, 
        ReferenceType, PointerType>*>(&base)));

    return static_cast<detail::AnyInputIterator_Base<ValueType, 
        ReferenceType, PointerType>&>(base).operator->();
}

template <typename ValueType, typename Reference,
          typename Pointer, typename DifferenceType>
inline any_iterator<std::output_iterator_tag, ValueType,
    Reference, Pointer, DifferenceType>& 
    any_iterator<std::output_iterator_tag, ValueType, Reference,
        Pointer, DifferenceType>::operator*() noexcept
{
    return *this;
}

template <typename ValueType, typename Reference,
          typename Pointer, typename DifferenceType>
inline const any_iterator<std::output_iterator_tag, ValueType,
    Reference, Pointer, DifferenceType>& 
    any_iterator<std::output_iterator_tag, ValueType, Reference,
        Pointer, DifferenceType>::operator*() const noexcept
{
    return *this;
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

template <typename ValueType, typename Reference,
          typename Pointer, typename DifferenceType>
template <typename U>
inline any_iterator<std::output_iterator_tag, ValueType, Reference, 
    Pointer, DifferenceType>& any_iterator<std::output_iterator_tag, ValueType,
    Reference, Pointer, DifferenceType>::operator=(U&& value)
{
    detail::AnyIterator_Base& base = BaseClass::base();
    assert((dynamic_cast<detail::AnyOutputIterator_Base<ValueType>*>(
        &base)));

    static_cast<detail::AnyOutputIterator_Base<ValueType>&>(base) = std::forward<U>(value);
    return *this;
}

template <typename ValueType, typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline any_iterator<std::forward_iterator_tag, ValueType, ReferenceType, PointerType,
                    DifferenceType> any_iterator<std::forward_iterator_tag, ValueType, 
                    ReferenceType, PointerType, DifferenceType>::operator++(int)
{
    auto tmp{*this};
    ++*this;
    return tmp;
}

template <typename ValueType, typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline any_iterator<std::bidirectional_iterator_tag, ValueType, ReferenceType, PointerType,
                    DifferenceType>& any_iterator<std::bidirectional_iterator_tag, ValueType, 
                    ReferenceType, PointerType, DifferenceType>::operator--()
{
    detail::AnyIterator_Base& baseIterator = base();
    assert((dynamic_cast<detail::AnyBidirectionalIterator_Base<ValueType, 
        ReferenceType, PointerType>*>(&baseIterator)));

    --static_cast<detail::AnyBidirectionalIterator_Base<ValueType, 
        ReferenceType, PointerType>&>(baseIterator);
    return *this;
}

template <typename ValueType, typename ReferenceType, typename PointerType, 
          typename DifferenceType>
inline any_iterator<std::bidirectional_iterator_tag, ValueType, ReferenceType, PointerType,
                    DifferenceType> any_iterator<std::bidirectional_iterator_tag, ValueType, 
                    ReferenceType, PointerType, DifferenceType>::operator--(int)
{
    auto tmp{*this};
    --*this;
    return tmp;
}

} // close namespace sample

#endif // SAMPLE_ANYITERATOR
