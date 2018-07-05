#ifndef SAMPLE_ANYBIDIRECTIONALITERATOR_BASE
#define SAMPLE_ANYBIDIRECTIONALITERATOR_BASE

#include <sample_anyforwarditerator_base.hpp>

namespace sample::detail {

template <typename ValueType, typename Reference, typename Pointer>
struct AnyBidirectionalIterator_Base : AnyForwardIterator_Base<ValueType, Reference, Pointer>
{
    // MUTATORS
    virtual AnyBidirectionalIterator_Base& operator--() = 0;
};

template <typename BiDirIt, typename ValueType, typename Reference, typename Pointer>
struct AnyBidirectionalIterator_Impl final : AnyBidirectionalIterator_Base<ValueType, 
    Reference, Pointer>
{
    // TYPES
    using value_type = ValueType;
    using reference = Reference;
    using pointer = Pointer;

    // CREATORS
    AnyBidirectionalIterator_Impl(BiDirIt it)
        noexcept(std::is_nothrow_copy_constructible_v<BiDirIt>);

    // ACCESSORS
    const void* base() const noexcept override;

    bool operator==(const AnyIterator_Base& rhs) const override;
    bool operator!=(const AnyIterator_Base& rhs) const override;

    reference operator*() const override;
    pointer operator->() const override;

    // MANIPULATORS
    void* base() noexcept override;

    AnyBidirectionalIterator_Impl& operator++() override;
    AnyBidirectionalIterator_Impl& operator--() override;

private:
    // DATA
    BiDirIt d_it;
};

template <typename ValueType, typename Reference, typename Pointer>
struct AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer> final
    : AnyBidirectionalIterator_Base<ValueType, Reference, Pointer>
{
    // TYPES
    using value_type = ValueType;
    using reference = Reference;
    using pointer = Pointer;

    // CREATORS
    constexpr AnyBidirectionalIterator_Impl() noexcept = default;

    // ACCESSORS
    const void* base() const noexcept override;

    bool operator==(const AnyIterator_Base& rhs) const override;
    bool operator!=(const AnyIterator_Base& rhs) const override;

    reference operator*() const override;
    pointer operator->() const override;

    // MANIPULATORS
    void* base() noexcept override;

    AnyBidirectionalIterator_Impl& operator++() override;
    AnyBidirectionalIterator_Impl& operator--() override;
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::AnyBidirectionalIterator_Impl(
    FwdIt it) noexcept(std::is_nothrow_copy_constructible_v<FwdIt>) 
    : d_it(it)
{}

// ACCESSORS
template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline const void* AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::base()
    const noexcept
{
    return static_cast<const void*>(&d_it);
}

template <typename ValueType, typename Reference, typename Pointer>
inline const void* AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::base()
    const noexcept
{
    return nullptr;
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline bool AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator==(
    const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyBidirectionalIterator_Impl*>(&rhs));
    const AnyBidirectionalIterator_Impl* const ptr = static_cast<const AnyBidirectionalIterator_Impl*>(&rhs);
    return d_it == ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer>
inline bool AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::operator==(
    const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyBidirectionalIterator_Impl*>(&rhs));
    return true;
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline bool AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator!=(
    const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyBidirectionalIterator_Impl*>(&rhs));
    const AnyBidirectionalIterator_Impl* const ptr = static_cast<const AnyBidirectionalIterator_Impl*>(&rhs);
    return d_it != ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer>
inline bool AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::operator!=(
    const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyBidirectionalIterator_Impl*>(&rhs));
    return false;
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline typename AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::reference
    AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator*() const
{
    return *d_it;
}

template <typename ValueType, typename Reference, typename Pointer>
inline typename AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::reference
    AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::operator*() const
{
    assert(false && "Cannot dereference a default constructed BidirectionalIterator");
}

template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline typename AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::pointer
    AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::operator->() const
{
    return std::addressof(*d_it);
}

template <typename ValueType, typename Reference, typename Pointer>
inline typename AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::pointer
    AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::operator->() const
{
    assert(false && "Cannot dereference a default constructed BidirectionalIterator");
}

// MANIPULATORS
template <typename FwdIt, typename ValueType, typename Reference, typename Pointer>
inline void* AnyBidirectionalIterator_Impl<FwdIt, ValueType, Reference, Pointer>::base()
    noexcept
{
    return static_cast<void*>(&d_it);
}

template <typename ValueType, typename Reference, typename Pointer>
inline void* AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::base()
    noexcept
{
    return nullptr;
}

template <typename BiDirIt, typename ValueType, typename Reference, typename Pointer>
inline AnyBidirectionalIterator_Impl<BiDirIt, ValueType, Reference, Pointer>&
    AnyBidirectionalIterator_Impl<BiDirIt, ValueType, Reference, Pointer>::operator++()
{
    ++d_it;
    return *this;
}

template <typename ValueType, typename Reference, typename Pointer>
inline AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>&
    AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::operator++()
{
    assert(false && "Cannot increment a default constructed BidirectionalIterator");
}

template <typename BiDirIt, typename ValueType, typename Reference, typename Pointer>
inline AnyBidirectionalIterator_Impl<BiDirIt, ValueType, Reference, Pointer>&
    AnyBidirectionalIterator_Impl<BiDirIt, ValueType, Reference, Pointer>::operator--()
{
    --d_it;
    return *this;
}

template <typename ValueType, typename Reference, typename Pointer>
inline AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>&
    AnyBidirectionalIterator_Impl<void, ValueType, Reference, Pointer>::operator--()
{
    assert(false && "Cannot decrement a default constructed BidirectionalIterator");
}

} // close namespace sample::detail

#endif // SAMPLE_ANYBIDIRECTIONALITERATOR_BASE
