#pragma once

#include <duke/base/Check.hpp>

#include <limits>
#include <initializer_list>

#include <cstdint>
#include <cstring>

/**
 * Vector implementation with small data optimization.
 *
 * This vector will not allocate memory if data is less than kSmallSize.
 * Default size ensures sizeof(SmallVector) == 32 and allows good
 * alignment and cache locality.
 */
template<typename T, typename SIZE = uint16_t, SIZE SmallSize = 32 - sizeof(SIZE)>
class SmallVector {
public:
    typedef const T* const_iterator;
    typedef T* iterator;
    typedef T value_type;

    SmallVector() {
    }
    SmallVector(size_t size) {
        allocate(size);
    }
    SmallVector(const T* pBegin, const T* pEnd) :
                    SmallVector(pEnd-pBegin) {
        set(pBegin);
    }
    SmallVector(const SmallVector& other) :
                    SmallVector(other.addressof(), other.addressof() + other.m_Size) {
    }
    SmallVector(SmallVector&& tmp) {
        memcpy(this, &tmp, sizeof(SmallVector<T> ));
        tmp.m_Size = 0;
    }
    SmallVector(std::initializer_list<T> il) :
                    SmallVector(il.begin(), il.end()) {
    }
    ~SmallVector() {
        deallocateIfNeeded();
    }

    SmallVector& operator=(const SmallVector& other) {
        allocate(other.m_Size);
        set(other.addressof());
        return *this;
    }

    bool operator==(const SmallVector& other) const {
        return m_Size == other.m_Size && memcmp(addressof(), other.addressof(), m_Size * sizeof(T)) == 0;
    }
    inline bool operator!=(const SmallVector& other) const {
        return !operator==(other);
    }
    bool operator<(const SmallVector& other) const {
        if (m_Size == other.m_Size) return memcmp(addressof(), other.addressof(), m_Size * sizeof(T)) < 0;
        return m_Size < other.m_Size;
    }

    inline size_t size() const {
        return m_Size;
    }
    inline T* data() {
        return addressof();
    }
    inline const T* data() const {
        return addressof();
    }
    inline const T* begin() const {
        return addressof();
    }
    inline const T* end() const {
        return addressof() + m_Size;
    }
    enum {
        kSmallSize = SmallSize
    };

    inline size_t elementSize() const {
        return m_Size * sizeof(T);
    }

    inline bool isAllocated() const {
        return elementSize() > kSmallSize;
    }
private:
    static_assert(kSmallSize>=sizeof(void*), "Small data should be pointer size at least");

    SIZE m_Size = 0;
    union {
        char m_SmallData[kSmallSize];
        T* ptr_;
    };

    inline const T* addressof() const {
        return isAllocated() ? ptr_ : &m_SmallData[0];
    }
    inline T* addressof() {
        return isAllocated() ? ptr_ : &m_SmallData[0];
    }

    void allocate(size_t size) {
        CHECK(size <= std::numeric_limits<SIZE>::max());
        deallocateIfNeeded();
        m_Size = size;
        fprintf(stderr, "%lu %s\n", size, isAllocated() ? "true" : "false");
        if (isAllocated()) ptr_ = reinterpret_cast<T*>(malloc(size * sizeof(T)));
    }

    void set(const T* pData) {
        memcpy(addressof(), pData, elementSize());
    }

    void deallocateIfNeeded() {
        if (isAllocated()) free(ptr_);
        ptr_ = nullptr;
    }
};
