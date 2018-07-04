#ifndef SAMPLE_ANYINPUTITERATOR_BASE
#define SAMPLE_ANYINPUTITERATOR_BASE

#include <sample_anyiterator_base.hpp>

#include <iterator>

namespace sample::detail {

template <typename ValueType, typename Reference = ValueType&,
          typename Pointer = ValueType*>
struct AnyInputIterator_Base : AnyIterator_Base {
    // ACCESSORS
    virtual Reference operator*() const = 0;
    virtual Pointer operator->() const = 0;
};

template <typename InputIt, typename ValueType,
          typename Reference, typename Pointer>
struct AnyInputIterator_Impl final 
    : AnyInputIterator_Base<ValueType, Reference, Pointer>
{ 
    // TYPES
    using value_type = ValueType;
    using reference = Reference;
    using pointer = Pointer;

    // CREATORS
    AnyInputIterator_Impl(InputIt it)
        noexcept(std::is_nothrow_copy_constructible_v<InputIt>);

    // ACCESSORS
    bool operator==(const AnyIterator_Base& rhs) const noexcept override;
    bool operator!=(const AnyIterator_Base& rhs) const noexcept override;

    reference operator*() const override;
    pointer operator->() const override;

    // MANIPULATORS
    AnyInputIterator_Impl& operator++() noexcept override;

private:
    // DATA
    InputIt d_it;
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
template <typename InputIt, typename ValueType, typename Reference, 
          typename Pointer>
inline AnyInputIterator_Impl<InputIt, ValueType, 
    Reference, Pointer>::AnyInputIterator_Impl(InputIt it)
    noexcept(std::is_nothrow_copy_constructible_v<InputIt>)
    : d_it(it)
{}

// ACCESSORS
template <typename InputIt, typename ValueType, typename Reference,
          typename Pointer>
inline bool AnyInputIterator_Impl<InputIt, ValueType, Reference,
    Pointer>::operator==(const AnyIterator_Base& rhs) const noexcept
{
    const AnyInputIterator_Impl* const ptr 
        = dynamic_cast<const AnyInputIterator_Impl*>(&rhs);
    return ptr ? d_it == ptr->d_it : false;
}

template <typename InputIt, typename ValueType, typename Reference,
          typename Pointer>
inline bool AnyInputIterator_Impl<InputIt, ValueType, Reference,
    Pointer>::operator!=(const AnyIterator_Base& rhs) const noexcept
{
    const AnyInputIterator_Impl* const ptr
        = dynamic_cast<const AnyInputIterator_Impl*>(&rhs);
    return ptr ? d_it != ptr->d_it : false;
}

template <typename InputIt, typename ValueType, typename Reference,
          typename Pointer>
inline typename AnyInputIterator_Impl<InputIt, ValueType,
    Reference, Pointer>::reference AnyInputIterator_Impl<InputIt,
    ValueType, Reference, Pointer>::operator*() const 
{
    return *d_it;
}

template <typename InputIt, typename ValueType, typename Reference,
          typename Pointer>
inline typename AnyInputIterator_Impl<InputIt, ValueType,
    Reference, Pointer>::pointer AnyInputIterator_Impl<InputIt,
    ValueType, Reference, Pointer>::operator->() const
{
    return d_it.operator->();
}

// MANIPULATORS
template <typename InputIt, typename ValueType, typename Reference,
          typename Pointer>
inline AnyInputIterator_Impl<InputIt, ValueType, Reference,
    Pointer>& AnyInputIterator_Impl<InputIt, ValueType,
    Reference, Pointer>::operator++() noexcept
{
    ++d_it;
    return *this;
}

} // close namespace sample::detail

#endif // SAMPLE_ANYINPUTITERATOR_BASE
