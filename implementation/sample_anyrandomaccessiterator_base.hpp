#ifndef SAMPLE_ANYRANDOMACCESSITERATOR_BASE
#define SAMPLE_ANYRANDOMACCESSITERATOR_BASE

#include <sample_anybidirectionaliterator_base.hpp>

namespace sample::detail {

template <typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
struct AnyRandomAccessIterator_Base 
    : AnyBidirectionalIterator_Base<ValueType, Reference, Pointer> 
{
    // ACCESSORS
    virtual Reference operator[](DifferenceType offset) const = 0;

    virtual DifferenceType operator-(const AnyRandomAccessIterator_Base& rhs) const = 0;

    virtual bool operator<(const AnyRandomAccessIterator_Base& rhs) const = 0;
    virtual bool operator>(const AnyRandomAccessIterator_Base& rhs) const = 0;

    virtual bool operator<=(const AnyRandomAccessIterator_Base& rhs) const = 0;
    virtual bool operator>=(const AnyRandomAccessIterator_Base& rhs) const = 0;

    // MANIPULATORS
    virtual AnyRandomAccessIterator_Base& operator+=(DifferenceType offset) = 0;
    virtual AnyRandomAccessIterator_Base& operator-=(DifferenceType offset) = 0;
};

template <typename RandIt, typename ValueType, typename Reference,
          typename Pointer, typename DifferenceType>
struct AnyRandomAccessIterator_Impl final
    : AnyRandomAccessIterator_Base<ValueType, Reference, Pointer, DifferenceType>
{
    // TYPES
    using value_type = ValueType;
    using reference = Reference;
    using pointer = Pointer;
    using difference_type = DifferenceType;

private:
    // PRIVATE TYPES
    using BaseClass = AnyRandomAccessIterator_Base<ValueType, Reference, 
        Pointer, DifferenceType>;

public:
    // CREATORS
    AnyRandomAccessIterator_Impl(RandIt it)
        noexcept(std::is_nothrow_copy_constructible_v<RandIt>);

    // ACCESSORS
    bool operator==(const AnyIterator_Base& rhs) const override;
    bool operator!=(const AnyIterator_Base& rhs) const override;

    bool operator<(const BaseClass& rhs) const override;
    bool operator>(const BaseClass& rhs) const override;

    bool operator<=(const BaseClass& rhs) const override;
    bool operator>=(const BaseClass& rhs) const override;

    reference operator*() const override;
    pointer operator->() const override;

    reference operator[](difference_type offset) const override;

    difference_type operator-(const BaseClass& rhs) const override;

    // MANIPULATORS
    AnyRandomAccessIterator_Impl& operator++() override;
    AnyRandomAccessIterator_Impl& operator--() override;
    AnyRandomAccessIterator_Impl& operator+=(difference_type offset) override;
    AnyRandomAccessIterator_Impl& operator-=(difference_type offset) override;

private:
    // DATA
    RandIt d_it;
};

template <typename ValueType, typename Reference,
          typename Pointer, typename DifferenceType>
struct AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, DifferenceType> final
    : AnyRandomAccessIterator_Base<ValueType, Reference, Pointer, DifferenceType>
{
    // TYPES
    using value_type = ValueType;
    using reference = Reference;
    using pointer = Pointer;
    using difference_type = DifferenceType;

    struct Key {};

private:
    // PRIVATE TYPES
    using BaseClass = AnyRandomAccessIterator_Base<ValueType, Reference, 
        Pointer, DifferenceType>;

public:
    // CREATORS
    constexpr AnyRandomAccessIterator_Impl(Key) noexcept;

    // ACCESSORS
    bool operator==(const AnyIterator_Base& rhs) const override;
    bool operator!=(const AnyIterator_Base& rhs) const override;

    bool operator<(const BaseClass& rhs) const override;
    bool operator>(const BaseClass& rhs) const override;

    bool operator<=(const BaseClass& rhs) const override;
    bool operator>=(const BaseClass& rhs) const override;

    reference operator*() const override;
    pointer operator->() const override;

    reference operator[](difference_type offset) const override;

    difference_type operator-(const BaseClass& rhs) const override;

    // MANIPULATORS
    AnyRandomAccessIterator_Impl& operator++() override;
    AnyRandomAccessIterator_Impl& operator--() override;
    AnyRandomAccessIterator_Impl& operator+=(difference_type offset) override;
    AnyRandomAccessIterator_Impl& operator-=(difference_type offset) override;
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
template <typename RandIt, typename ValueType, typename Reference,
          typename Pointer, typename DifferenceType>
inline AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::AnyRandomAccessIterator_Impl(RandIt it)
        noexcept(std::is_nothrow_copy_constructible_v<RandIt>)
    : d_it(it)
{}

template <typename ValueType, typename Reference,
          typename Pointer, typename DifferenceType>
inline constexpr AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer,
    DifferenceType>::AnyRandomAccessIterator_Impl(Key) noexcept
{}

// ACCESSORS
template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator==(const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    const AnyRandomAccessIterator_Impl* const ptr = static_cast<const AnyRandomAccessIterator_Impl*>(&rhs);
    return d_it == ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator==(const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    return true;
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer, 
          typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator!=(const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    const AnyRandomAccessIterator_Impl* const ptr = static_cast<const AnyRandomAccessIterator_Impl*>(&rhs);
    return d_it != ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator!=(const AnyIterator_Base& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    return false;
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator<(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    const AnyRandomAccessIterator_Impl* const ptr = static_cast<const AnyRandomAccessIterator_Impl*>(&rhs);
    return d_it < ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator<(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    return false;
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator>(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    const AnyRandomAccessIterator_Impl* const ptr = static_cast<const AnyRandomAccessIterator_Impl*>(&rhs);
    return d_it > ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator>(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    return false;
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator<=(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    const AnyRandomAccessIterator_Impl* const ptr = static_cast<const AnyRandomAccessIterator_Impl*>(&rhs);
    return d_it <= ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator<=(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    return true;
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator>=(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    const AnyRandomAccessIterator_Impl* const ptr = static_cast<const AnyRandomAccessIterator_Impl*>(&rhs);
    return d_it >= ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline bool AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator>=(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    return true;
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer, 
          typename DifferenceType>
inline typename AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer,
    DifferenceType>::reference AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer,
    DifferenceType>::operator*() const
{
    return *d_it;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline typename AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::reference AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer,
    DifferenceType>::operator*() const
{
    assert(false && "Cannot dereference a default constructed RandomAccessIterator");
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer, 
          typename DifferenceType>
inline typename AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer,
    DifferenceType>::pointer AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer,
    DifferenceType>::operator->() const
{
    return std::addressof(*d_it);
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline typename AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer,
    DifferenceType>::pointer AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer,
    DifferenceType>::operator->() const
{
    assert(false && "Cannot dereference a default constructed RandomAccessIterator");
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer, 
          typename DifferenceType>
inline typename AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer,
    DifferenceType>::reference AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer,
    DifferenceType>::operator[](difference_type offset) const
{
    return d_it[offset];
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline typename AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::reference AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer,
    DifferenceType>::operator[](difference_type offset) const
{
    assert(false && "Cannot dereference a default constructed RandomAccessIterator");
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer, 
          typename DifferenceType>
inline typename AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer,
    DifferenceType>::difference_type AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer,
    DifferenceType>::operator-(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs));
    const AnyRandomAccessIterator_Impl* const ptr = static_cast<const AnyRandomAccessIterator_Impl*>(&rhs);
    return d_it - ptr->d_it;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline typename AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::difference_type AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer,
    DifferenceType>::operator-(const BaseClass& rhs) const
{
    assert(dynamic_cast<const AnyRandomAccessIterator_Impl*>(&rhs)); // Can only subtract another
                                                                     // default-constructed iterator.
    return true;
}

// MANIPULATORS
template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, DifferenceType>&
    AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator++()
{
    ++d_it;
    return *this;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, DifferenceType>&
    AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator++()
{
    assert(false && "Cannot increment a default constructed RandomAccessIterator");
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, DifferenceType>&
    AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator--()
{
    --d_it;
    return *this;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, DifferenceType>&
    AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator--()
{
    assert(false && "Cannot decrement a default constructed RandomAccessIterator");
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, DifferenceType>&
    AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator+=(difference_type offset)
{
    d_it += offset;
    return *this;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, DifferenceType>&
    AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator+=(difference_type)
{
    assert(false && "Cannot increment a default constructed RandomAccessIterator");
}

template <typename RandIt, typename ValueType, typename Reference, typename Pointer,
          typename DifferenceType>
inline AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, DifferenceType>&
    AnyRandomAccessIterator_Impl<RandIt, ValueType, Reference, Pointer, 
    DifferenceType>::operator-=(difference_type offset)
{
    d_it -= offset;
    return *this;
}

template <typename ValueType, typename Reference, typename Pointer, typename DifferenceType>
inline AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, DifferenceType>&
    AnyRandomAccessIterator_Impl<void, ValueType, Reference, Pointer, 
    DifferenceType>::operator-=(difference_type)
{
    assert(false && "Cannot increment a default constructed RandomAccessIterator");
}


} // close namespace sample::detail

#endif // SAMPLE_ANYRANDOMACCESSITERATOR_BASE
