# Type Erased Iterators for modern C++

__Author__: Thomas Russell \<thomas.russell97@googlemail.com\>  
__Date__: 2nd July 2018  
__Target__: Library Evolution Working Group (LEWG)

## Abstract
This paper proposes adding the template class `std::any_iterator` - a type-erased iterator which can be used where physical encapsulation is desired but the generality of iterators is beneficial. It also proposes the helper alias templates: `std::any_input_iterator`, `std::any_output_iterator`, `std::any_forward_iterator`, `std::any_bidirectional_iterator` and `std::any_random_access_iterator`.

## Table of Concepts
- [Motivation](#motivation)
  - [Example Usage](#example-usage)
  - [Memory Allocation](#memory-allocation)
  - [User-defined Types](#user-defined-types)
  - [Alternatives](#alternatives)
- [Prior Art](#prior-art)
- [Impact on the standard](#impact-on-the-standard)
- [Wording](#wording)
  - [Synopsis](#synopsis)
  - [Specification](#specification)

## Motivation
Iterators (and soon, ranges also) are a powerful and generic abstraction idiom in C++. They allow library and application developers to write their functions restricted only on the properties that they actually need (traversal requirements, dereference type, etc.).

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

Generic programming techniques (such as accepting arbitrary iterator types through templates) also prevent various ubiquitous OOP techniques such as inheritance and run-time polymorphism, again making some developers reluctant to adopt them.

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

### Memory Allocation
It is worth noting that as with any type-erased mechanism, allocation is often needed (the existance of a small-buffer optimization can help to mitigate this, but never remove it).

Following the example given by removal of the `std::allocator_arg_t` constructors for `std::function` and their lack of inclusion in `std::any`, we have not included them for `std::any_iterator`.

It is possible that polymorphic memory resources, i.e. `std::pmr::memory_resource`-derived classes could be useful in providing customizable allocation behaviour. However, as this was neither used in design of `std::any` nor retroactively applied to `std::function` it is not a part of this proposal.

### User-defined Types
Whilst I anticipate that most use-cases will be satisfied with the existing `any_iterator` which is fully compatible with the STL iterator categories, there are projects which extend, or use different iterator categories. For these projects (such as Boost.Iterator), it would be a valid customization point for them to specialize the `std::any_iterator` class for their iterator category as it would be a UDT. 

### Alternatives
The following are alternative solutions to this problem:

- Instead of having a single `std::any_iterator` with member functions using SFINAE to remove operations that are not appropriate for each particular class of iterator, have 5 separate classes (`std::any_input_iterator`, `std::any_output_iterator`, `std::any_forward_iterator`, `std::any_bidirectional_iterator`, `std::any_random_access_iterator`).
- Provide a more generic type-erasure mechanism within the standard library that would make it trivial for users to write their own correct and performant `any_iterator`. Perhaps this would take the form of something like:  
  ```c++
  std::opaque<Concept> obj(somethingModellingConcept);
  ```

  Where `std::opaque` would deduce from `Concept` the necessary members. This would require extensive changes to the core language and would provide a much more generic and powerful tool than proposed here. It may not even be possible.

## Prior Art
Analogous classes exist in various other places within the C++ community:

- Thomas Becker's `any_iterator` 
- Adobe's ASL has a suite of type-erased iterator classes for each of the STL iterator tags.
- Boost has an `any_iterator` internally which is used to implement Boost.Ranges `any_range`.
- As an example of how to use the `dyno` library, there is an `any_iterator` class.
- There is an ACCU article from July 2000 detailing how to create a simple `any_iterator` class.

## Impact on the standard
This proposal is a pure library extension. It requires addition of a new standard library header `<any_iterator>`, no modifications to other headers are required.

### Interaction with Ranges
It is worth noting at this point that both Boost.Range and ranges v3 have an `any_range` class, which acts as a type-erased range adapter. It is likely that such a class would be added to the ISO C++ standard at a later date, in this event, having a pre-existing `any_iterator` would ease the burden of implementation on standard library vendors and ease burden of specification on LEWG.

## Wording
### Synopsis
```c++
namespace std {
    template <typename IteratorCategory, typename ValueType,
          typename ReferenceType, typename PointerType,
          typename DifferenceType>
    struct any_iterator {
        // TYPES
        using value_type = ValueType;
        using reference = ReferenceType;
        using pointer = PointerType;
        using difference_type = DifferenceType;
        using iterator_category = IteratorCategory;

        // CREATORS
        any_iterator() noexcept;
            // SFINAE'd out unless ForwardIterator

        any_iterator(const any_iterator&);
        any_iterator(any_iterator&&);

        template <typename It>
        any_iterator(It it);

        template <typename OtherCategory, typename OtherValue, 
                typename OtherReference, typename OtherPointer,
                typename OtherDifferenceType>
        any_iterator(const any_iterator<OtherCategory, OtherValue, OtherReference, OtherPointer, OtherDifferenceType>& other);
        template <typename OtherCategory, typename OtherValue, 
                typename OtherReference, typename OtherPointer,
                typename OtherDifferenceType>
        any_iterator(any_iterator<OtherCategory, OtherValue, OtherReference, OtherPointer, OtherDifferenceType>&& other);
        ~any_iterator();

        // OBSERVERS
        void* base() const noexcept;

        reference operator*() const;
        /*unspecified*/ operator->() const;
            // SFINAE'd out unless InputIterator

        reference operator[](difference_type offset) const;
        any_iterator operator+(difference_type offset) const;
        any_iterator operator-(difference_type offset) const;
        difference_type operator-(const any_iterator& rhs) const;
            // SFINAE'd out unless RandomAccessIterator

        // COMPARISON OPERATORS
        bool operator==(const any_iterator& rhs) const;
        bool operator!=(const any_iterator& rhs) const;
            // SFINAE'd out unless InputIterator

        bool operator<(const any_iterator& rhs) const;
        bool operator>(const any_iterator& rhs) const;
        bool operator<=(const any_iterator& rhs) const;
        bool operator>=(const any_iterator& rhs) const;
            // SFINAE'd out unless RandomAccessIterator

        // MODIFIERS
        void swap(any_iterator& other);

        any_iterator& operator++();

        any_iterator& operator*();
            // SFINAE'd out unless OutputIterator
        any_iterator& operator=(value_type value);
            // SFINAE'd out unless OutputIterator

        any_iterator operator++(int);
            // SFINAE'd out unless ForwardIterator

        any_iterator& operator--();
        any_iterator operator--(int);
            // SFINAE'd out unless BidirectionalIterator

        any_iterator& operator+=(difference_type offset);
        any_iterator& operator-=(difference_type offset);
            // SFINAE'd out unless RandomAccessIterator

    private:
        BufferType d_buffer; // Exposition Only
    };


    // Alias templates
    template <typename ValueType,
              typename Reference = ValueType&, 
              typename Pointer = ValueType*, 
              typename DifferenceType = std::ptrdiff_t>
    using any_input_iterator = any_iterator<std::input_iterator_tag, ValueType, Reference, Pointer, DifferenceType>;
    template <typename ValueType,
              typename Reference = ValueType&, 
              typename Pointer = ValueType*, 
              typename DifferenceType = std::ptrdiff_t>
    using any_output_iterator = any_iterator<std::output_iterator_tag, ValueType, Reference, Pointer, DifferenceType>;
    template <typename ValueType,
              typename Reference = ValueType&, 
              typename Pointer = ValueType*, 
              typename DifferenceType = std::ptrdiff_t>
    using any_forward_iterator = any_iterator<std::forward_iterator_tag, ValueType, Reference, Pointer, DifferenceType>;
    template <typename ValueType,
              typename Reference = ValueType&, 
              typename Pointer = ValueType*, 
              typename DifferenceType = std::ptrdiff_t>
    using any_bidirectional_iterator = any_iterator<std::bidirectional_iterator_tag, ValueType, Reference, Pointer, DifferenceType>;
    template <typename ValueType,
              typename Reference = ValueType&, 
              typename Pointer = ValueType*, 
              typename DifferenceType = std::ptrdiff_t>
    using any_random_access_iterator = any_iterator<std::random_access_iterator_tag, ValueType, Reference, Pointer, DifferenceType>;
}
```

### Specification
#### Class template `any_iterator` [any_iterator]
##### Class template `any_iterator` constructors [any_iterator.ctor]
`any_iterator() noexcept`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Constructs an empty `any_iterator`, equivalent to the singular iterator for `ForwardIterator`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This constructor shall not participate in overload resolution unless `iterator_category` is derived from `input_iterator_tag`.

`any_iterator(const any_iterator& other)`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Copy-constructs an `any_iterator` from `other`, such that the underlying iterator is formed by copy-construction from the underlying iterator of `other`.

`any_iterator(any_iterator&& other)`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Move-constructs an `any_iterator` from `other`, such that the underlying iterator is formed by move-construction from the underlying iterator of `other`.

`template <typename It> any_iterator(It it)`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Constructs an `any_iterator` with a type-erased underlying iterator of type `It` move-constructed from `it`.   
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: `typename std::iterator_traits<It>::iterator_category` must be derived from `iterator_category`.

##### Class template `any_iterator` observers [any_iterator.observers]
`void* base() const noexcept`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Returns a pointer to the underlying iterator whose type has been erased by this `any_iterator`. If called on a singular iterator, this will return the null pointer.

`reference operator*() const;`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Returns a `reference` to the result of dereferencing the underlying iterator.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and contain a dereferencible underlying iterator. Otherwise the behaviour is undefined.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `input_iterator_tag`.

`/*unspecified*/ operator->() const;`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Returns an unspecified object which implements `operator->` such that the end result is as if the user had called `operator->` directly on the underlying iterator.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and contain a dereferencible underlying iterator. Otherwise the behaviour is undefined.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `input_iterator_tag`.

`reference operator[](difference_type offset) const;`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Returns the equivalent of the expression `*(*this + offset)`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and upon being advanced by `offset` shall be dereferencable; otherwise the behaviour is undefined.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

`any_iterator operator+(difference_type offset) const`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Returns an `any_iterator` containing the result of `UnderlyingIterator(*this) + offset`.   
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and upon being advanced by `offset` shall be valid; otherwise the behaviour is undefined.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

`any_iterator operator-(difference_type offset) const`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Returns an `any_iterator` containing the result of `UnderlyingIterator(*this) - offset`.   
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and upon retreating by `offset` shall be valid; otherwise the behaviour is undefined.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

`difference_type operator-(const any_iterator& rhs) const`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Returns the distance between `*this` and `rhs` as computed by using `operator-` on their underlying iterators.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The underlying iterators of `*this` and `rhs` must be of the same type and it must be valid to call `operator-` with both as arguments.    
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

##### Class template `any_iterator` comparison operators [any_iterator.compare]
`bool operator==(const any_iterator& rhs) const;`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Compares the underlying iterators of `*this` and `rhs` for equality.    
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and the underlying iterators of `*this` and `rhs` should have the same type and be equality comparable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `input_iterator_tag`.


`bool operator!=(const any_iterator& rhs) const;`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Compares the underlying iterators of `*this` and `rhs` for inequality.    
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and the underlying iterators of `*this` and `rhs` should have the same type and be equality comparable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `input_iterator_tag`.


`bool operator<(const any_iterator& rhs) const;`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Compares the underlying iterators of `*this` and `rhs` using `operator<`.    
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and the underlying iterators of `*this` and `rhs` should have the same type and be less-than comparable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

`bool operator>(const any_iterator& rhs) const;`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Compares the underlying iterators of `*this` and `rhs` using `operator>`.    
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and the underlying iterators of `*this` and `rhs` should have the same type and be greater-than comparable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

`bool operator<=(const any_iterator& rhs) const;`
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Compares the underlying iterators of `*this` and `rhs` using `operator<=`.    
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and the underlying iterators of `*this` and `rhs` should have the same type and be less-than and equality comparable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

`bool operator>=(const any_iterator& rhs) const;`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Compares the underlying iterators of `*this` and `rhs` using `operator>=`.    
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The `any_iterator` shall be valid and the underlying iterators of `*this` and `rhs` should have the same type and be greater-than and equality comparable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

##### Class template `any_iterator` modifiers [any_iterator.modify]
`void swap(any_iterator& other)`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Swaps the underlying iterators of `*this` and `other`.

`any_iterator& operator++()`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Increments the underlying iterator of `*this`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The underlying iterator should be incrementable.

`any_iterator& operator*()`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: No-op, returns a reference to `*this`.    
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `output_iterator_tag`.

`any_iterator& operator=(value_type value)`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Assigns `value` to the underlying iterator as if by `*UnderlyingIterator(*this) = std::move(value)`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The underlying iterator shall be valid and assignable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `output_iterator_tag`.

`any_iterator operator++(int)`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Makes a copy of `*this`, increments `*this` then returns the copy.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The underlying iterator shall be incrementable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `forward_iterator_tag`.

`any_iterator& operator--()`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Decrements the underlying iterator of `*this`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The underlying iterator shall be decrementable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `bidirectional_iterator_tag`.

`any_iterator operator--(int)`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Makes a copy of `*this`, decrements `*this` then returns the copy.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: The underlying iterator shall be decrementable.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `bidirectional_iterator_tag`.

`any_iterator& operator+=(difference_type offset)`  
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Advances the underlying iterator of `*this` by `offset` by calling `operator+=`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: It is valid to advance the underlying iterator of `*this` by `offset`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.

`any_iterator& operator-=(difference_type offset)`
&nbsp;&nbsp;&nbsp;&nbsp;_Effects_: Retreats the underlying iterator of `*this` by `offset` by calling `operator-=`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Requires_: It is valid to retreat the underlying iterator of `*this` by `offset`.  
&nbsp;&nbsp;&nbsp;&nbsp;_Remarks_: This operator shall not participate in overload resolution unless `iterator_category` is derived from `random_access_iterator_tag`.
