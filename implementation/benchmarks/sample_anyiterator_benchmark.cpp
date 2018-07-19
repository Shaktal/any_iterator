#include <sample_anyiterator.hpp>

#include <benchmark/benchmark.h>

#include <array>

namespace {
    template <typename It>
    void BM_IteratorCreation(benchmark::State& state);
    template <typename It>
    void BM_IteratorCopy(benchmark::State& state);
    template <typename It>
    void BM_IteratorCopyToOutput(benchmark::State& state);
    template <typename OutIt>
    void BM_IteratorOutputIt(benchmark::State& state);

    using ContainerType = std::vector<int>;
    constexpr std::size_t N = 200u;
}

BENCHMARK_TEMPLATE(BM_IteratorCreation, sample::any_input_iterator<int>)->Arg(N);
BENCHMARK_TEMPLATE(BM_IteratorCreation, ContainerType::iterator)->Arg(N);
BENCHMARK_TEMPLATE(BM_IteratorCopy, sample::any_input_iterator<int>)->Arg(N);
BENCHMARK_TEMPLATE(BM_IteratorCopy, ContainerType::iterator)->Arg(N);
BENCHMARK_TEMPLATE(BM_IteratorCopyToOutput, sample::any_input_iterator<int>)->Arg(N);
BENCHMARK_TEMPLATE(BM_IteratorCopyToOutput, ContainerType::iterator)->Arg(N);
BENCHMARK_TEMPLATE(BM_IteratorOutputIt, sample::any_output_iterator<int>)->Arg(N);
BENCHMARK_TEMPLATE(BM_IteratorOutputIt, std::back_insert_iterator<ContainerType>)->Arg(N);

namespace {
template <typename It, typename... Args>
[[gnu::noinline]] It CreateIterator(Args&&... args)
{
    if constexpr (std::is_aggregate_v<It>)
    {
        return It{std::forward<Args>(args)...};
    }
    else
    {
        return It(std::forward<Args>(args)...);
    }
}

template <typename It>
void BM_IteratorCreation(benchmark::State& state)
{
    ContainerType input(state.range(0));

    while (state.KeepRunning())
    {
        auto it = CreateIterator<It>(begin(input));
    }
}

template <typename It>
void BM_IteratorCopy(benchmark::State& state)
{
    ContainerType input(state.range(0));
    auto it = CreateIterator<It>(begin(input));
    while (state.KeepRunning())
    {
        auto copy = CreateIterator<It>(it);
    }
}

template <typename It>
void BM_IteratorCopyToOutput(benchmark::State& state)
{
    ContainerType input(state.range(0));
    ContainerType output(state.range(0));

    while (state.KeepRunning())
    {
        auto first = CreateIterator<It>(begin(input));
        auto last = CreateIterator<It>(end(input));
        auto d_first = CreateIterator<It>(begin(output));

        std::copy(first, last, d_first);
    }
}

template <typename OutIt>
void BM_IteratorOutputIt(benchmark::State& state)
{
    ContainerType input(state.range(0));
    ContainerType output;
    output.reserve(state.range(0));

    while (state.KeepRunning())
    {
        auto d_first = CreateIterator<OutIt>(std::back_inserter(output));
        std::copy(begin(input), end(input), d_first);
    }
}
} // close anonymous namespace

BENCHMARK_MAIN();
