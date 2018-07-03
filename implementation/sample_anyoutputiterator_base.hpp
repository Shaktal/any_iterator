#ifndef SAMPLE_ANYOUTPUTITERATOR_BASE
#define SAMPLE_ANYOUTPUTITERATOR_BASE

#include <type_traits>

namespace sample::detail {

template <typename OutputType>
struct AnyOutputIterator_Base {
    // ACCESSORS
    virtual bool operator==(const AnyOutputIterator_Base&) const noexcept = 0;

    AnyOutputIterator_Base& operator*() const noexcept;

    // MANIPULATORS
    virtual AnyOutputIterator_Base& operator=(OutputType&& value) = 0;
    virtual AnyOutputIterator_Base& operator=(const OutputType& value) = 0;
    virtual AnyOutputIterator_Base& operator++() noexcept = 0;
};

template <typename OutputIt, typename OutputType>
struct AnyOutputIterator_Impl final : AnyOutputIterator_Base<OutputType>
{ 
    // CREATORS
    AnyOutputIterator_Impl(OutputIt it)
        noexcept(std::is_nothrow_copy_constructible_v<OutputIt>);

    // ACCESSORS
    bool operator==(const AnyOutputIterator_Base<OutputType>& rhs) const 
        noexcept override;

    // MANIPULATORS
    AnyOutputIterator_Impl& operator=(OutputType&& value) override;
    AnyOutputIterator_Impl& operator=(const OutputType& value) override;
    AnyOutputIterator_Impl& operator++() noexcept override;

private:
    // DATA
    OutputIt d_it;
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
                // =================================
                // class AnyOutputIterator_Base
                // =================================

// ACCESSORS
template <typename OutputType>
inline AnyOutputIterator_Base<OutputType>& 
    AnyOutputIterator_Base<OutputType>::operator*() const noexcept
{
    return *this;
}

                // =================================
                // class AnyOutputIterator_Impl
                // =================================
// CREATORS
template <typename OutputIt, typename OutputType>
inline AnyOutputIterator_Impl<OutputIt, OutputType>::AnyOutputIterator_Impl(
    OutputIt it) noexcept(std::is_nothrow_copy_constructible_v<OutputIt>)
    : d_it(it)
{}

// ACCESSORS
template <typename OutputIt, typename OutputType>
inline bool AnyOutputIterator_Impl<OutputIt, OutputType>::operator==(
    const AnyOutputIterator_Base<OutputType>& rhs) const noexcept
{
    const AnyOutputIterator_Impl* const ptr 
        = dynamic_cast<const AnyOutputIterator_Impl*>(rhs);
    return ptr ? this->d_it == ptr->d_it : false;
}

// MANIPULATORS
template <typename OutputIt, typename OutputType>
inline AnyOutputIterator_Impl<OutputIt, OutputType>& 
    AnyOutputIterator_Impl<OutputIt, OutputType>::operator=(OutputType&& value)
{
    *d_it = std::move(value);
}

template <typename OutputIt, typename OutputType>
inline AnyOutputIterator_Impl<OutputIt, OutputType>& 
    AnyOutputIterator_Impl<OutputIt, OutputType>::operator=(const OutputType& value)
{
    *d_it = value;
}

template <typename OutputIt, typename OutputType>
inline AnyOutputIterator_Impl<OutputIt, OutputType>& 
    AnyOutputIterator_Impl<OutputIt, OutputType>::operator++()
{
    ++d_it;
    return *this;
}


} // close namespace sample::detail

#endif // SAMPLE_ANYOUTPUTITERATOR_BASE
