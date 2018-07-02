#ifndef SAMPLE_ANYOUTPUTITERATOR_BASE
#define SAMPLE_ANYOUTPUTITERATOR_BASE

#include <type_traits>

namespace sample::detail {

template <typename ValueType, typename Reference = ValueType&,
          typename Pointer = ValueType*>
struct AnyOutputIterator_Base {
    // ACCESSORS
    virtual bool operator==(const AnyOutputIterator_Base&) const noexcept = 0;

    virtual Reference operator*() const = 0;
    virtual Pointer operator->() const = 0;

    // MANIPULATORS
    virtual AnyOutputIterator_Base& operator++() noexcept = 0;
};

template <typename OutputIt>
struct AnyOutputIterator_Impl final 
    : AnyOutputIterator_Base<
        typename std::iterator_traits<OutputIt>::value_type,
        typename std::iterator_traits<OutputIt>::reference,
        typename std::iterator_traits<OutputIt>::pointer
      >
{ 
    // CREATORS
    AnyOutputIterator_Impl(OutputIt it)
        noexcept(std::is_nothrow_copy_constructible_v<OutputIt>);

    // ACCESSORS
    bool operator==(const AnyOutputIterator_Base& rhs) const noexcept override;

    decltype(*std::declval<OutputIt&>()) operator*() const override;
    decltype(std::declval<OutputIt&>().operator->()) operator->() const override;

    // MANIPULATORS
    AnyOutputIterator_Impl& operator++() noexcept override;

private:
    // DATA
    OutputIt d_it;
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
template <typename OutputIt>
inline AnyOutputIterator_Impl<OutputIt>::AnyOutputIterator_Impl(OutputIt it)
    noexcept(std::is_nothrow_copy_constructible_v<OutputIt>)
    : d_it(it)
{}

// ACCESSORS
template <typename OutputIt>
inline bool AnyOutputIterator_Impl<OutputIt>::operator==(
    const AnyOutputIterator_Base& rhs) const noexcept
{
    
}

} // close namespace sample::detail

#endif // SAMPLE_ANYOUTPUTITERATOR_BASE
