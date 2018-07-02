#ifndef SAMPLE_ANYINPUTITERATOR_BASE
#define SAMPLE_ANYINPUTITERATOR_BASE

#include <sample_anyiterator_base.hpp>

namespace sample::detail {

template <typename ValueType, typename Reference = ValueType&,
          typename Pointer = ValueType*>
struct AnyInputIterator_Base : AnyIterator_Base {
    // ACCESSORS
    virtual Reference operator*() const = 0;
    virtual Pointer operator->() const = 0;
};

template <typename InputIt>
struct AnyInputIterator_Impl final 
    : AnyInputIterator_Base<
        typename std::iterator_traits<InputIt>::value_type,
        typename std::iterator_traits<InputIt>::reference,
        typename std::iterator_traits<InputIt>::pointer
      >
{ 
    // TYPES
    using value_type = typename std::iterator_traits<InputIt>::value_type;
    using reference = typename std::iterator_traits<InputIt>::reference;
    using pointer = typename std::iterator_traits<InputIt>::pointer;

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
template <typename InputIt>
inline AnyInputIterator_Impl<InputIt>::AnyInputIterator_Impl(InputIt it)
    noexcept(std::is_nothrow_copy_constructible_v<InputIt>)
    : d_it(it)
{}

// ACCESSORS
template <typename InputIt>
inline bool AnyInputIterator_Impl<InputIt>::operator==(
    const AnyIterator_Base& rhs) const noexcept
{
    const AnyInputIterator_Impl* const ptr 
        = dynamic_cast<const AnyInputIterator_Impl*>(&rhs);
    return ptr ? d_it == ptr->d_it : false;
}

template <typename InputIt>
inline bool AnyInputIterator_Impl<InputIt>::operator!=(
    const AnyIterator_Base& rhs) const noexcept
{
    const AnyInputIterator_Impl* const ptr
        = dynamic_cast<const AnyInputIterator_Impl*>(&rhs);
    return ptr ? d_it == ptr->d_it : false;
}

template <typename InputIt>
inline typename AnyInputIterator_Impl<InputIt>::reference 
    AnyInputIterator_Impl<InputIt>::operator*() const 
{
    return *d_it;
}

template <typename InputIt>
inline typename AnyInputIterator_Impl<InputIt>::pointer
    AnyInputIterator_Impl<InputIt>::operator->() const
{
    return d_it.operator->();
}

// MANIPULATORS
template <typename InputIt>
inline AnyInputIterator_Impl<InputIt>&
    AnyInputIterator_Impl<InputIt>::operator++() noexcept
{
    ++d_it;
    return *this;
}

} // close namespace sample::detail

#endif // SAMPLE_ANYINPUTITERATOR_BASE
