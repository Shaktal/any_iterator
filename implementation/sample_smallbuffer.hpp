#ifndef SAMPLE_SMALLBUFFER_HPP
#define SAMPLE_SMALLBUFFER_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <experimental/memory_resource>
#include <memory>
#include <new>
#include <utility>
#include <type_traits>

namespace sample::detail {

namespace pmr = std::experimental::pmr;

constexpr std::size_t DEFAULT_BUFFER_SIZE = 64ul;

template <typename BaseType, std::size_t BufferSize = DEFAULT_BUFFER_SIZE>
struct SmallBuffer;

template <typename BaseType, std::size_t BufferSize>
void swap(SmallBuffer<BaseType, BufferSize>& lhs, 
          SmallBuffer<BaseType, BufferSize>& rhs);

template <typename BaseType, std::size_t BufferSize>
struct SmallBuffer {
    // TYPES
    using allocator_type = pmr::memory_resource;

    // CREATORS
    SmallBuffer(const SmallBuffer& rhs, allocator_type* alloc = nullptr);
    SmallBuffer(SmallBuffer&& rhs, allocator_type* alloc = nullptr);
    template <typename T, typename... Args>
    SmallBuffer(std::in_place_type_t<T>, Args&&... args);
    template <typename T, typename... Args>
    SmallBuffer(std::allocator_arg_t, allocator_type* alloc,
                std::in_place_type_t<T>, Args&&... args);
    template <typename OtherBase, std::size_t OtherBufferSize,
              typename = std::enable_if_t<std::is_base_of_v<BaseType, OtherBase>>>
    SmallBuffer(const SmallBuffer<OtherBase, OtherBufferSize>& rhs,
                allocator_type* alloc = nullptr);
    template <typename OtherBase, std::size_t OtherBufferSize,
              typename = std::enable_if_t<std::is_base_of_v<BaseType, OtherBase>>>
    SmallBuffer(SmallBuffer<OtherBase, OtherBufferSize>&& rhs,
                allocator_type* alloc = nullptr);
    ~SmallBuffer();

    // ACCESSORS
    BaseType& operator*() const noexcept;
    BaseType* operator->() const noexcept;

    allocator_type* allocator() const noexcept;

    // MANIPULATORS
    void swap(SmallBuffer& other);

private:
    // PRIVATE TYPES
    using BufferType = std::aligned_storage_t<BufferSize, alignof(BaseType)>;
    using CloneFunc = BaseType*(*)(const BaseType*, std::byte*, std::size_t, 
        pmr::memory_resource*);
    using MoveFunc = BaseType*(*)(BaseType*&, std::byte*, std::size_t,
        pmr::memory_resource*, pmr::memory_resource*);
    using DeleteFunc = void(*)(BaseType*, pmr::memory_resource*);
    using AllocType = allocator_type;

private:
    // FRIENDS
    template <typename OtherBase, std::size_t OtherSize>
    friend class SmallBuffer;

private:
    // DATA
    AllocType*  d_alloc;

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

namespace {
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
                 std::size_t bufferSize, pmr::memory_resource* resource)
{
    assert(original);
    assert(resource);

    static_assert(std::is_base_of_v<BaseType, T>);
    const T* const cast_original = static_cast<const T*>(original);

    std::byte* const alignedTarget = nextAlignedAddress<T>(targetBuffer);
    const std::ptrdiff_t offset = alignedTarget - targetBuffer;
    bufferSize -= offset;

    if (bufferSize >= sizeof(T)) {
        new ((void*)alignedTarget) T(*cast_original);
        return reinterpret_cast<BaseType*>(alignedTarget);
    } else {
        pmr::polymorphic_allocator<T> alloc{resource};
        T* const target = alloc.allocate(1u);
        alloc.construct(target, *cast_original);
        return target;
    }
}

template <typename BaseType, typename T, bool NeedsNullify>
BaseType* mover(BaseType*& original, std::byte* targetBuffer, 
                std::size_t bufferSize, pmr::memory_resource* resource1,
                pmr::memory_resource* resource2)
{
    assert(original);
    assert(resource1);
    assert(resource2);

    static_assert(std::is_base_of_v<BaseType, T>);
    T* const cast_original = static_cast<T*>(original);

    std::byte* const alignedTarget = nextAlignedAddress<T>(targetBuffer);
    const std::ptrdiff_t offset = alignedTarget - targetBuffer;
    bufferSize -= offset;

    if (bufferSize >= sizeof(T)) {
        new ((void*)alignedTarget) T(std::move(*cast_original));
        return reinterpret_cast<BaseType*>(alignedTarget);
    } else {
        if constexpr (NeedsNullify) {
            if (*resource1 == *resource2) {
                BaseType* copy = original;
                original = nullptr;
                return copy;
            }
        }

        pmr::polymorphic_allocator<T> alloc{resource1};
        T* const target = alloc.allocate(1u);
        alloc.construct(target, std::move(*cast_original));
        return target;
    }
}

template <typename BaseType, typename T, bool NeedsDelete>
void deleter(BaseType* obj, pmr::memory_resource* resource)
{
    assert(resource);
    if (!obj) return;

    if constexpr (NeedsDelete) {
        pmr::polymorphic_allocator<T> alloc{resource};
        alloc.destroy(static_cast<T*>(obj));
        alloc.deallocate(static_cast<T*>(obj), 1u);
    } else {
        static_cast<T*>(obj)->~T();
    }
}
} // close anonymous namespace

// CREATORS
template <typename BaseType, std::size_t BufferSize>
template <typename T, typename... Args>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(std::allocator_arg_t,
    allocator_type* allocator, std::in_place_type_t<T>, Args&&... args)
    : d_alloc(allocator ? allocator : pmr::get_default_resource())
    , d_cloner(&cloner<BaseType, std::decay_t<T>>)
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
        pmr::polymorphic_allocator<decayed_type> alloc{d_alloc};
        d_type = alloc.allocate(1u);
        d_mover = &mover<BaseType, decayed_type, true>;
        d_deleter = &deleter<BaseType, decayed_type, true>;
        alloc.construct(static_cast<decayed_type*>(d_type), 
            std::forward<Args>(args)...);
        return;
    }

    new ((void*)address) decayed_type(std::forward<Args>(args)...);
    d_type = reinterpret_cast<BaseType*>(address);
}

template <typename BaseType, std::size_t BufferSize>
template <typename T, typename... Args>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(std::in_place_type_t<T>, 
    Args&&... args)
    : SmallBuffer(std::allocator_arg, nullptr, std::in_place_type<T>,
                  std::forward<Args>(args)...)
{}

template <typename BaseType, std::size_t BufferSize>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(const SmallBuffer& rhs,
    pmr::memory_resource* allocator)
    : d_alloc(allocator ? allocator : pmr::get_default_resource())
    , d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_cloner(rhs.d_type,
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize,
        d_alloc))
{}

template <typename BaseType, std::size_t BufferSize>
template <typename OtherBase, std::size_t OtherBufferSize, typename>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(
    const SmallBuffer<OtherBase, OtherBufferSize>& rhs,
    pmr::memory_resource* allocator)
    : d_alloc(allocator ? allocator : pmr::get_default_resource())
    , d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_cloner(rhs.d_type, 
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize,
        d_alloc))
{
}

template <typename BaseType, std::size_t BufferSize>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(SmallBuffer&& rhs,
    pmr::memory_resource* allocator)
    : d_alloc(allocator ? allocator : pmr::get_default_resource())
    , d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_mover(rhs.d_type, 
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize,
        d_alloc, rhs.allocator()))
{
}

template <typename BaseType, std::size_t BufferSize>
template <typename OtherBase, std::size_t OtherBufferSize, typename>
inline SmallBuffer<BaseType, BufferSize>::SmallBuffer(
    SmallBuffer<OtherBase, OtherBufferSize>&& rhs,
    pmr::memory_resource* allocator)
    : d_alloc(allocator ? allocator : pmr::get_default_resource())
    , d_cloner(rhs.d_cloner)
    , d_mover(rhs.d_mover)
    , d_deleter(rhs.d_deleter)
    , d_type(d_mover(rhs.d_type, 
        reinterpret_cast<std::byte*>(std::addressof(d_storage)), BufferSize,
        d_alloc, rhs.allocator()))
{
}

template <typename BaseType, std::size_t BufferSize>
inline SmallBuffer<BaseType, BufferSize>::~SmallBuffer()
{
    d_deleter(d_type, d_alloc);
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

template <typename BaseType, std::size_t BufferSize>
inline typename SmallBuffer<BaseType, BufferSize>::allocator_type* 
    SmallBuffer<BaseType, BufferSize>::allocator() const noexcept
{
    return d_alloc;
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
