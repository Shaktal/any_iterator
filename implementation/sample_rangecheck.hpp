#ifndef SAMPLE_RANGECHECK
#define SAMPLE_RANGECHECK

#include <limits>

namespace sample::detail {

template <typename From, typename To>
constexpr bool range_check() noexcept;
    // Check that all of the values expressible in the type `From` 
    // are expressible in the type `To`.

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
template <typename From, typename To>
inline constexpr bool range_check() noexcept
{
    constexpr bool min_bound = 
        (std::numeric_limits<From>::min() >= std::numeric_limits<To>::min());
    constexpr bool max_bound = 
        (std::numeric_limits<From>::max() <= std::numeric_limits<To>::max());
    return min_bound && max_bound;
}

} // close namespace sample::detail

#endif // SAMPLE_RANGECHECK
