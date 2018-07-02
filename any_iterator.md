# Type Erased Iterators for modern C++

__Author__: Thomas Russell \<thomas.russell97@googlemail.com\>  
__Date__: 2nd July 2018  
__Target__: Library Evolution Working Group (LEWG)

## Abstract
This paper proposes adding the template class `std::any_iterator` - a type-erased iterator which can be used where physical encapsulation is desired but the genericity of iterators is beneficial. It also proposes the helper alias templates: `std::any_input_iterator`, `std::any_output_iterator`, `std::any_forward_iterator`, `std::any_bidirectional_iterator` and `std::any_random_access_iterator`.

## Table of Concepts
- Motivation  
  - Example Usage
  - Allocation
  - User-defined Types
- Prior Art
- Interaction with other areas of the Standard
- Wording

## Motivation
Iterators (and soon, ranges also) are a powerful and generic abstraction idiomatic in C++. They allow library and application developers to write their functions restricted only on the properties that they actually need (traversal requirements, dereference type, etc.).

However, when looking through a production code-base, it is not uncommon to see code that looks like the following:

```c++
// application_algorithm.h
struct SomeUDT {
    void getSomeObjects(std::vector<Object>& objects,
        const std::vector<std::string>& names);
};

// application_algorithm.cpp
void SomeUDT::getSomeObjects(std::vector<Object>& objects, 
    const std::vector<std::string>& names)
{
    for (const auto& name : names) {
        Object myObject = getObject(name);
        objects.push_back(std::move(myObject));
    }
}

// some_caller.cpp
std::map<std::string_view, Object> objectMap;
SomeUDT udt;

template <auto Projector>
using ProjectionIterator = projection_iterator<Projector>;
using ValueType = decltype(objectMap)::value_type;
std::vector<std::string> names{
    ProjectionIterator<&ValueType::first>(begin(objectMap)),
    ProjectionIterator<&ValueType::first>(end(objectMap))};
std::vector<Object> results;
udt.getSomeObjects(results, names);

std::copy(
    std::make_move_iterator(begin(results)), 
    std::make_move_iterator(end(results)),
    ProjectionIterator<&ValueType::second>(begin(objectMap)));
```

This is, of course, unnecessarily restrictive on the caller of this algorithm; the implementation of `getSomeObjects` doesn't use the fact that `std::vector` is contiguous, they are just interested in having some output range in which to put `Object` objects. It also requires that I have a contiguous vector of `std::string` objects to call it. If I have a range of valid `std::string_view` objects, I have to jump through hoops to use this function, and impose a potentially significant performance penalty.

On the other hand, we are told when designing enterprise-scale software to employ physical design principals. Having to declare the majority of our algorithms as template functions breaks physical encapsulation and can result in complaints of code-bloat, ugliness and increased compilation-time. Thus we are often reduced to writing code for the most common case; using concrete types and being overly restrictive.

Using type-erasure is the idiomatic way to solve this problem in C++, and we have several existing vocabulary types for solving other problem classes (`std::function`, `std::any`). In this proposal, we consider the addition of a `std::any_iterator` template class that solves this problem for the case of iterators.

### Example Usage
Consider the previous example code. With the existance of a `std::any_iterator` template class, this would instead look something like:

```c++
// application_algorithm.h
struct SomeUDT {
    void getSomeObjects(
        std::any_output_iterator<Object> d_outputObjects,
        std::any_input_iterator<std::string_view> namesFirst,
        std::any_input_iterator<std::string_view> namesLast);
};

// application_algorithm.cpp
void SomeUDT::getSomeObjects(
    std::any_output_iterator<Object> d_outputObjects, 
    std::any_input_iterator<std::string_view> namesFirst,
    std::any_input_iterator<std::string_view> namesLast)
{
    while (namesFirst != namesLast) {
        *d_outputObjects++ = getObject(*namesFirst);
        ++namesFirst;
    }
}

// some_caller.cpp
std::map<std::string_view, Object> objectMap;
SomeUDT udt;

template <auto Projector>
using ProjectionIterator = projection_iterator<Projector>;
using ValueType = decltype(objectMap)::value_type;
udt.getSomeObjects(
    ProjectionIterator<&ValueType::second>(begin(objectMap)), 
    ProjectionIterator<&ValueType::first>(begin(objectMap)),
    ProjectionIterator<&ValueType::first>(end(objectMap)));
```

We have shifted most of the complexity from the caller to the callee, reducing duplication of effort by consumers of this component. We have done this without breaking any pre-existing physical encapsulation. In this case, we may even get a non-negligible performance improvement from not having to construct a `std::string` for each of the names.

It is worth noting that implicit conversion can occur along the standard iterator hierarchy (e.g. an `any_random_access_iterator` can be used to construct a `any_bidirectional_iterator`, which in turn can be used to construct a `any_output_iterator`).

Here we have also made use of the alias templates which are also proposed here:

- `std::any_input_iterator`
- `std::any_output_iterator`
- `std::any_forward_iterator`
- `std::any_bidirectional_iterator`
- `std::any_random_access_iterator`

_Note that these are formally defined in the wording section below._

### Allocation
It is worth noting that as with any type-erased mechanism, allocation is often needed (the existance of a small-buffer optimization can help to mitigate this, but never remove it).

Following the example given by removal of the `std::allocator_arg_t` constructors for `std::function` we have not included them for `std::any_iterator`.

### User Defined Types
Whilst I anticipate that most use-cases will be satisfied with the specializations of `any_iterator` for the STL iterator categories, there are projects which extend, or use different iterator categories. For these projects (such as Boost.Iterator), it would be a valid customization point for them to specialize the `std::any_iterator` class for their iterator category as it would be a UDT. 

## Prior Art
Analogous classes exist in several other open-source projects:

- Thomas Becker's `any_iterator` 
- Adobe's ASL has a suite of type-erased iterator classes for each of the STL iterator tags.
- Boost has an `any_iterator` internally which is used to implement Boost.Ranges `any_range`.

## Interaction with other areas of the Standard
### Core Language
No change to the language is necessary to facilitate this feature.

### Library 
This is a stand-alone library class, requiring no change to any other area of the library. 

#### Interaction with Concepts
It is worth noting that both Boost.Range and ranges v3 have an `any_range` class, which acts as a type-erased range. It is likely that such a class would be added to the ISO C++ standard at a later date, in this event, having a pre-existing `any_iterator` would ease the burden of implementation on standard library vendors.

## Wording
### Synopsis
```c++
namespace std {
    template <typename IteratorCategory,
              typename ValueType,
              typename Reference = ValueType&,
              typename Pointer = ValueType*,
              typename DifferenceType = std::ptrdiff_t>
    struct any_iterator {
        using value_type = ValueType;
        using reference = Reference;
        using pointer = Pointer;
        using difference_type = DifferenceType;
        using iterator_category = IteratorCategory;

        any_iterator() noexcept = default;
        any_iterator(const any_iterator&) = default;
        ~any_iterator() = default;

        bool operator==(const any_iterator&) const;
        bool operator!=(const any_iterator&) const;

        void swap(any_iterator&) noexcept;

        any_iterator& operator++();

    private:
        SmallBuffer d_buffer; // Exposition Only
    }; 

    // Specialization for modelling the `InputIterator` concept.
    template <typename ValueType,
              typename Reference = ValueType&,
              typename Pointer = ValueType*,
              typename DifferenceType = std::ptrdiff_t>
    struct any_iterator<std::input_iterator_tag, ValueType, Reference, Pointer, DifferenceType> 
        : any_iterator<void, ValueType, Reference, Pointer, DifferenceType>
    {
        using iterator_category = std::input_iterator_tag;

        template <typename IteratorCategory2,
            typename ValueType2, typename ReferenceType2,
            typename PointerType2, typename DifferenceType2>
        any_iterator(const any_iterator<IteratorCategory2>&);
        template <typename InputIt>
        any_iterator(InputIt it);

        Reference operator*() const;
        Pointer operator->() const;
    };

    // Specialization for modelling the `OutputIterator` concept.
    template <typename ValueType,
              typename Reference = ValueType&,
              typename Pointer = ValueType*,
              typename DifferenceType = std::ptrdiff_t>
    struct any_iterator<std::input_iterator_tag, ValueType, Reference, Pointer, DifferenceType> 
        : any_iterator<void, ValueType, Reference, Pointer, DifferenceType>
    {
        using iterator_category = std::input_iterator_tag;

        template <typename IteratorCategory2,
            typename ValueType2, typename ReferenceType2,
            typename PointerType2, typename DifferenceType2>
        any_iterator(const any_iterator<IteratorCategory2>&);
        template <typename InputIt>
        any_iterator(InputIt it);

        template <typename U>
        any_iterator& operator=(U&& value);

        any_iterator& operator*() const noexcept;
    };

    // Specialization for modelling the `ForwardIterator` concept.
    template <typename ValueType,
              typename Reference = ValueType&,
              typename Pointer = ValueType*,
              typename DifferenceType = std::ptrdiff_t>
    struct any_iterator<std::forward_iterator, ValueType, Reference, Pointer, DifferenceType> 
        : any_iterator<std::input_iterator, ValueType, Reference, Pointer, DifferenceType>
    {
        using iterator_category = std::input_iterator_tag;

        template <typename IteratorCategory2,
            typename ValueType2, typename ReferenceType2,
            typename PointerType2, typename DifferenceType2>
        any_iterator(const any_iterator<IteratorCategory2>&);
        template <typename InputIt>
        any_iterator(InputIt it);

        template <typename U>
        any_iterator& operator=(U&& value);

        any_iterator& operator*() const noexcept;
    };
}
```