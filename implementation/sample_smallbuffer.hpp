#ifndef SAMPLE_SMALLBUFFER_HPP
#define SAMPLE_SMALLBUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <new>
#include <type_traits>

namespace sample::detail {

constexpr std::size_t DEFAULT_BUFFER_SIZE = 256ul;

template <typename BaseType, std::size_t BufferSize = DEFAULT_BUFFER_SIZE>
struct SmallBuffer {
    // CREATORS
    template <typename T>
    SmallBuffer(T&& arg);
    template <typename OtherBase, std::size_t OtherBufferSize,
              typename = std::enable_if_t<std::is_base_of_v<BaseType, OtherBase>>
    SmallBuffer(const SmallBuffer<OtherBase, OtherBufferSize>&);
    template <typename OtherBase, std::size_t OtherBufferSize,
              typename = std::enable_if_t<std::is_base_of_v<BaseType, OtherBase>>
    SmallBuffer(SmallBuffer<OtherBase, OtherBufferSize>&&);
    ~SmallBuffer();

    // ACCESSORS
    BaseType& operator*() const noexcept;
    BaseType* operator->() const noexcept;

private:
    // PRIVATE TYPES
    using BufferType = std::aligned_storage_t<BufferSize, alignof(BaseType)>;
    using CloneFunc = BaseType*(*)(const BaseType*, std::byte*, std::size_t);
    using MoveFunc = BaseType*(*)(BaseType*, std::byte*, std::size_t);
    using DeleteFunc = void(*)(BaseType*);

private:
    // FRIENDS
    template <typename OtherBase, std::size_t OtherSize>
    friend class SmallBuffer;

private:
    // DATA
    CloneFunc   d_cloner;
    MoveFunc    d_mover;
    DeleteFunc  d_deleter;
    BufferType  d_storage;
    BaseType*   d_type;
};

// ===========================================================================
//      INLINE DEFINITIONS
// ===========================================================================
// FREE FUNCTIONS
template <typename T>
constexpr std::byte* nextAlignedAddress(std::byte* address) noexcept
{
    const std::uintptr_t value = reinterpret_cast<std::uintptr_t>(address);
    if (auto offset = value % alignof(T); offset) {
        return address + (alignof(T) - offset);
    } 
    return address;
}

template <typename T>
BaseType* cloner(const BaseType* original, std::byte* targetBuffer, 
                 std::size_t bufferSize)
{
    static_assert(std::is_base_of_v<BaseType, T>);
    const T* const cast_original = static_cast<const T*>(original);

    std::byte* const alignedTarget = nextAlignedAddress<T>(targetBuffer);
    const std::ptrdiff_t offset = alignedTarget - targetBuffer;
    bufferSize -= offset;

    if (bufferSize >= sizeof(T)) {
        new ((void*)alignedTarget) T(*cast_original);
        return reinterpret_cast<BaseType*>(alignedTarget);
    } else {
        return new T(*cast_original);
    }
}

template <typename T>
BaseType* mover(BaseType* original, std::byte* targetBuffer, 
                std::size_t bufferSize)
{
    static_assert(std::is_base_of_v<BaseType, T>);
    T* const cast_original = static_cast<T*>(original);

    std::byte* const alignedTarget = nextAlignedAddress<T>(targetBuffer);
    const std::ptrdiff_t offset = alignedTarget - targetBuffer;
    bufferSize -= offset;

    if (bufferSize >= sizeof(T)) {
        new ((void*)alignedTarget) T(std::move(*cast_original));
        return reinterpret_cast<BaseType*>(alignedTarget);
    } else {
        return new T(std::move(*cast_original));
    }
}

template <typename T>
void deleter(BaseType* obj)
{
    obj->~T();
}

// CREATORS
template <typename BaseType, std::size_t BufferSize>
template <typename T>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(T&& arg)
    : d_cloner(&cloner<std::decay_t<T>>)
    , d_mover(&mover<std::decay_t<T>>)
    , d_deleter(&deleter<std::decay_t<T>>)
{
    using decayed_type = std::decay_t<T>;
    std::byte* const address 
        = nextAlignedAddress<decayed_type>(std::addressof(d_storage));

    if (BufferSize - (address - std::addressof(d_storage)) < sizeof(T)) {
        d_type = new T(std::forward<T>(arg));
        return;
    }

    new ((void*)address) T(std::forward<T>(arg));
}

template <typename BaseType, std::size_t BufferSize>
template <typename OtherBase, std::size_t OtherBufferSize>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(
    const SmallBuffer<OtherBase, OtherBufferSize>& rhs)
    : d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_cloner(rhs.d_type, std::addressof(d_storage), BufferSize))
{}

template <typename BaseType, std::size_t BufferSize>
template <typename OtherBase, std::size_t OtherBufferSize>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(
    SmallBuffer<OtherBase, OtherBufferSize>&& rhs)
    : d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_mover(rhs.d_type, std::addressof(d_storage), BufferSize))
{}

template <typename BaseType, std::size_t BufferSize>
inline SmallBuffer<BaseType, BufferSize>::~SmallBuffer()
{
    if (d_type > (std::addressof(d_storage) + BufferSize) &&
        d_type < std::addressof(d_storage))
    {
        delete d_type;
        return;
    } 

    d_deleter(d_type);
}

// ACCESSORS
template <typename BaseType, std::size_t BufferSize>
inline BaseType& SmallBuffer<BaseType, BufferSize>::operator*() const noexcept
{
    return *d_type;
}

template <typename BaseType, std::size_t BufferSize>
inline BaseType* SmallBuffer<BaseType, BufferSize>::operator->() const noexcept
{
    return d_type;
}
} // close namespace sample::detail

#endif // SAMPLE_SMALLBUFFER_HPP
