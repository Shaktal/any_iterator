#ifndef SAMPLE_ANYITERATOR_BASE
#define SAMPLE_ANYITERATOR_BASE

namespace sample::detail {

struct AnyIterator_Base {
    // This class provides a common base for all of the other
    // iterator bases

    // ACCESSORS
    virtual bool operator==(const AnyIterator_Base&) const = 0;
    virtual bool operator!=(const AnyIterator_Base&) const = 0;

    // MANIPULATORS
    virtual AnyIterator_Base& operator++() noexcept = 0;
};

} // close namespace sample::detail

#endif // SAMPLE_ANYITERATOR_BASE
