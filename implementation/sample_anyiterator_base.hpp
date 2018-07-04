#ifndef SAMPLE_ANYITERATOR_BASE
#define SAMPLE_ANYITERATOR_BASE

namespace sample::detail {

struct AnyIterator_Base {
    // This class provides a common base for all of the other
    // iterator bases
    
    // CREATORS
    virtual ~AnyIterator_Base() = 0;

    // MANIPULATORS
    virtual AnyIterator_Base& operator++() = 0;
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// CREATORS
inline AnyIterator_Base::~AnyIterator_Base() = default;

} // close namespace sample::detail

#endif // SAMPLE_ANYITERATOR_BASE
