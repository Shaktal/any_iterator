# Type Erased Iterators for modern C++

This repository contains a proposal to add `any_iterator` and some helper alias templates to the C++ standard library. 

## Where is the Proposal?
The proposal paper can be found as "any_iterator.md" in this respository ([link here](https://github.com/Shaktal/any_iterator/blob/master/any_iterator.md)).

## Is there a sample implementation?
Yes, there is a sample implementation of that proposal within the "implementation" folder.

To set-up and build the sample implementation and tests, perform the following steps:

```
git clone git@github.com:Shaktal/any_iterator.git any_iterator
cd any_iterator/implementation && cmake . && make AnyIteratorTests && ./AnyIteratorTests
```

This will build the test suite for the sample implementation. Please feel free to add test cases as desired and submit them in a PR, or alternatively, raise any issues that you discover.

Comments on the proposal paper are also welcomed: please raise them as issues on the github page.

### Example Usage
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

### Benchmarks
There are some simple microbenchmarks included using Google Benchmark. These are provided to give some perspective on the relative performance of using the iterator directly or using an `any_iterator`. 

These can be built by cloning the repository as above and initializing CMake (preferably using the `-DCMAKE_BUILD_TYPE=Release` flag) and then calling `make AnyIteratorBenchmarks`.

On my machine (Intel® Core™ i7-3630QM CPU @ 2.40GHz × 8, 7.7GiB DDR4 RAM) I get the following results:

```
---------------------------------------------------------------------------------------------------------
Benchmark                                                                  Time           CPU Iterations
---------------------------------------------------------------------------------------------------------
BM_IteratorCreation<sample::any_input_iterator<int>>/200                   6 ns          6 ns  111440708
BM_IteratorCreation<ContainerType::iterator>/200                           1 ns          1 ns  819729215
BM_IteratorCopy<sample::any_input_iterator<int>>/200                       6 ns          6 ns  112859931
BM_IteratorCopy<ContainerType::iterator>/200                               0 ns          0 ns 1000000000
BM_IteratorCopyToOutput<sample::any_input_iterator<int>>/200            1267 ns       1266 ns     527775
BM_IteratorCopyToOutput<ContainerType::iterator>/200                      23 ns         23 ns   30847233
BM_IteratorOutputIt<sample::any_output_iterator<int>>/200               1356 ns       1355 ns     571798
BM_IteratorOutputIt<std::back_insert_iterator<ContainerType>>/200        761 ns        760 ns     905092
```
