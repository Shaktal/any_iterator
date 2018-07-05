#ifndef SAMPLE_SMALLBUFFER_HPP
#define SAMPLE_SMALLBUFFER_HPP

#include <cstddef>
#include <cstdint>
#include <new>
#include <iostream>
#include <type_traits>

namespace sample::detail {

constexpr std::size_t DEFAULT_BUFFER_SIZE = 64ul;

template <typename BaseType, std::size_t BufferSize = DEFAULT_BUFFER_SIZE>
struct SmallBuffer;

template <typename BaseType, std::size_t BufferSize>
void swap(SmallBuffer<BaseType, BufferSize>& lhs, 
          SmallBuffer<BaseType, BufferSize>& rhs);

template <typename BaseType, std::size_t BufferSize>
struct SmallBuffer {
    // TYPES
    template <typename T> struct Key {};

    // CREATORS
    SmallBuffer(const SmallBuffer& rhs);
    SmallBuffer(SmallBuffer&& rhs);
    template <typename T, typename... Args>
    SmallBuffer(Key<T>, Args&&... args);
    template <typename OtherBase, std::size_t OtherBufferSize,
              typename = std::enable_if_t<std::is_base_of_v<BaseType, OtherBase>>>
    SmallBuffer(const SmallBuffer<OtherBase, OtherBufferSize>& rhs);
    template <typename OtherBase, std::size_t OtherBufferSize,
              typename = std::enable_if_t<std::is_base_of_v<BaseType, OtherBase>>>
    SmallBuffer(SmallBuffer<OtherBase, OtherBufferSize>&& rhs);
    ~SmallBuffer();

    // ACCESSORS
    BaseType& operator*() const noexcept;
    BaseType* operator->() const noexcept;

    // MANIPULATORS
    void swap(SmallBuffer& other);

private:
    // PRIVATE TYPES
    using BufferType = std::aligned_storage_t<BufferSize, alignof(BaseType)>;
    using CloneFunc = BaseType*(*)(const BaseType*, std::byte*, std::size_t);
    using MoveFunc = BaseType*(*)(BaseType*&, std::byte*, std::size_t);
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
template <typename BaseType, std::size_t BufferSize>
void swap(SmallBuffer<BaseType, BufferSize>& lhs,
          SmallBuffer<BaseType, BufferSize>& rhs)
{
    lhs.swap(rhs);
}

template <typename T>
constexpr std::byte* nextAlignedAddress(std::byte* address) noexcept
{
    const std::uintptr_t value = reinterpret_cast<std::uintptr_t>(address);
    if (auto offset = value % alignof(T); offset) {
        return address + (alignof(T) - offset);
    } 
    return address;
}

template <typename BaseType, typename T>
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

template <typename BaseType, typename T, bool NeedsNullify>
BaseType* mover(BaseType*& original, std::byte* targetBuffer, 
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

    if constexpr (NeedsNullify) {
        original = nullptr;
    }
}

template <typename BaseType, typename T, bool NeedsDelete>
void deleter(BaseType* obj)
{
    if constexpr (NeedsDelete) {
        delete obj;
    } else {
        static_cast<T*>(obj)->~T();
    }
}

// CREATORS
template <typename BaseType, std::size_t BufferSize>
template <typename T, typename... Args>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(Key<T>, Args&&... args)
    : d_cloner(&cloner<BaseType, std::decay_t<T>>)
    , d_mover(&mover<BaseType, std::decay_t<T>, false>)
    , d_deleter(&deleter<BaseType, std::decay_t<T>, false>)
{
    using decayed_type = std::decay_t<T>;
    static_assert(std::is_base_of_v<BaseType, decayed_type>);

    std::byte* const storageAddress 
        = reinterpret_cast<std::byte*>(std::addressof(d_storage));
    std::byte* const address 
        = nextAlignedAddress<decayed_type>(
            reinterpret_cast<std::byte*>(storageAddress));

    if (BufferSize - (address - storageAddress) < sizeof(T)) {
        d_type = new decayed_type(std::forward<Args>(args)...);
        d_mover = &mover<BaseType, decayed_type, true>;
        d_deleter = &deleter<BaseType, decayed_type, true>;
        return;
    }

    new ((void*)address) decayed_type(std::forward<Args>(args)...);
    d_type = reinterpret_cast<BaseType*>(address);
}

template <typename BaseType, std::size_t BufferSize>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(const SmallBuffer& rhs)
    : d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_cloner(rhs.d_type,
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize))
{}

template <typename BaseType, std::size_t BufferSize>
template <typename OtherBase, std::size_t OtherBufferSize, typename>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(
    const SmallBuffer<OtherBase, OtherBufferSize>& rhs)
    : d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_cloner(rhs.d_type, 
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize))
{
    // TODO: need to set deleter appropriately
}

template <typename BaseType, std::size_t BufferSize>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(SmallBuffer&& rhs)
    : d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_mover(rhs.d_type, 
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize))
{
    // TODO: Need to set deleter appropriately.
}

template <typename BaseType, std::size_t BufferSize>
template <typename OtherBase, std::size_t OtherBufferSize, typename>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(
    SmallBuffer<OtherBase, OtherBufferSize>&& rhs)
    : d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_mover(rhs.d_type, 
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize))
{
    // TODO: Need to set deleter appropriately.
}

template <typename BaseType, std::size_t BufferSize>
inline SmallBuffer<BaseType, BufferSize>::~SmallBuffer()
{
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

// MANIPULATORS
template <typename BaseType, std::size_t BufferSize>
inline void SmallBuffer<BaseType, BufferSize>::swap(SmallBuffer& other)
{
    auto tmp{std::move(*this)};
    d_cloner = other.d_cloner;
    d_mover = other.d_mover;
    d_deleter = other.d_deleter;
    d_type = d_mover(other.d_type, 
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize);
    
    other.d_cloner = tmp.d_cloner;
    other.d_mover = tmp.d_mover;
    other.d_deleter = tmp.d_deleter;
    other.d_type = other.d_mover(tmp.d_type,
        reinterpret_cast<std::byte*>(std::addressof(other.d_storage)), 
        BufferSize);
}
} // close namespace sample::detail

#endif // SAMPLE_SMALLBUFFER_HPP
