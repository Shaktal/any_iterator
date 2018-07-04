# Type Erased Iterators for modern C++

This repository contains a proposal to add `any_iterator` and some helper alias templates to the C++ standard library. 

## Where is the Proposal?
The proposal paper can be found as "any_iterator.md" in this respository ([link here](https://github.com/Shaktal/any_iterator/blob/master/any_iterator.md)).

## Is there a sample implementation?
Yes, there is a sample implementation of that proposal within the "implementation" folder.

To set-up and build the sample implementation and tests, perform the following steps:

```
git clone git@github.com:Shaktal/any_iterator.git any_iterator
cd any_iterator/implementation && cmake . && make && ./AnyIterator
```

This will build the test suite for the sample implementation. Please feel free to add test cases as desired and submit them in a PR, or alternatively, raise any issues that you discover.

Comments on the proposal paper are also welcomed: please raise them as issues on the github page.

## Example
An example program using the `any_iterator` sample implementation might look something like the following:

```c++
using namespace sample;
void capitalize(any_input_iterator<std::string> first,
                any_input_iterator<std::string> last,
                any_output_iterator<std::string> d_outputs)
{
    std::transform(first, last, d_outputs, [](auto str) {
        // Ignore the `::toupper` potential UB ;)
        std::transform(begin(str), end(str), begin(str), ::toupper);
        return str;
    });
}

int main()
{
    capitalize(std::istream_iterator<std::string>(std::cin),
               std::istream_iterator<std::string>(),
               std::ostream_iterator<std::string>(std::cout, "\n"));
}
```
