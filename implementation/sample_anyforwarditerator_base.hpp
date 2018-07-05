#ifndef SAMPLE_ANYFORWARDITERATOR_BASE
#define SAMPLE_ANYFORWARDITERATOR_BASE

#include <sample_anyinputiterator_base.hpp>

#include <cassert>
#include <type_traits>

namespace sample::detail {

template <typename ValueType, typename Reference, typename Pointer>
struct AnyForwardIterator_Base : AnyInputIterator_Base<ValueType, Reference, Pointer> 
{};

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
struct AnyForwardIterator_Impl final : AnyForwardIterator_Base<ValueType, Reference, Pointer>
{
    // TYPES
    using value_type = ValueType;
    using reference = Reference;
    using pointer = Pointer;

    // CREATORS
    AnyForwardIterator_Impl(FwdIt it)
        noexcept(std::is_nothrow_copy_constructible_v<FwdIt>);

    // ACCESSORS
    const void* base() const noexcept override;

    bool operator==(const AnyIterator_Base& rhs) const override;
    bool operator!=(const AnyIterator_Base& rhs) const override;

    reference operator*() const override;
    pointer operator->() const override;

    // MANIPULATORS
    void* base() noexcept override;

    AnyForwardIterator_Impl& operator++() override;

private:
    // DATA
    FwdIt d_it;
};

template <typename ValueType, typename Reference, typename Pointer>
struct AnyForwardIterator_Impl<void, ValueType, Reference, Pointer> final 
    : AnyForwardIterator_Base<ValueType, Reference, Pointer>
{
    // TYPES
    using value_type = ValueType;
    using reference = Reference;
    using pointer = Pointer;

    // CREATORS
    constexpr AnyForwardIterator_Impl() noexcept = default;

    // ACCESSORS
    const void* base() const noexcept override;

    bool operator==(const AnyIterator_Base& rhs) const override;
    bool operator!=(const AnyIterator_Base& rhs) const override;

    [[noreturn]] reference operator*() const override;
    [[noreturn]] pointer operator->() const override;

    // MANIPULATORS
    void* base() noexcept override;

    [[noreturn]] AnyForwardIterator_Impl& operator++() override;
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::AnyForwardIterator_Impl(
    FwdIt it) noexcept(std::is_nothrow_copy_constructible_v<FwdIt>) 
    : d_it(it)
{}

// ACCESSORS
template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline const void* AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::base()
    const noexcept
{
    return static_cast<const void*>(&d_it);
}

template <typename ValueType, typename Reference, typename Pointer>
inline const void* AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::base() 
    const noexcept
{
    return nullptr;
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline bool AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator==(
    const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyForwardIterator_Impl*>(&rhs));
    const AnyForwardIterator_Impl* const ptr = static_cast<const AnyForwardIterator_Impl*>(&rhs);
    return d_it == ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer>
inline bool AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::operator==(
    const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyForwardIterator_Impl*>(&rhs));
    return true;
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline bool AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator!=(
    const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyForwardIterator_Impl*>(&rhs));
    const AnyForwardIterator_Impl* const ptr = static_cast<const AnyForwardIterator_Impl*>(&rhs);
    return d_it != ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer>
inline bool AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::operator!=(
    const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyForwardIterator_Impl*>(&rhs));
    return false;
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline typename AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::reference
    AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator*() const
{
    return *d_it;
}

template <typename ValueType, typename Reference, typename Pointer>
inline typename AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::reference
    AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::operator*() const
{
    assert(false && "Cannot dereference a default constructed ForwardIterator");
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline typename AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::pointer
    AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator->() const
{
    return std::addressof(*d_it);
}

template <typename ValueType, typename Reference, typename Pointer>
inline typename AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::pointer
    AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::operator->() const
{
    assert(false && "Cannot dereference a default constructed ForwardIterator");
}

// MANIPULATORS
template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline void* AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::base()
    noexcept
{
    return static_cast<void*>(&d_it);
}

template <typename ValueType, typename Reference, typename Pointer>
inline void* AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::base() 
    noexcept
{
    return nullptr;
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>&
    AnyForwardIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator++()
{
    ++d_it;
    return *this;
}

template <typename ValueType, typename Reference, typename Pointer>
inline AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>&
    AnyForwardIterator_Impl<void, ValueType, Reference, Pointer>::operator++()
{
    assert(false && "Cannot increment a default constructed ForwardIterator");
}

} // close namespace sample::detail

#endif // SAMPLE_ANYFORWARDITERATOR_BASE
