#pragma once

#include <cstring>

/**
 * Vector implementation with small data optimization.
 *
 * This vector will not allocate memory if data is less than kSmallSize.
 * Default size ensures sizeof(SmallVector) == 32 and allows good
 * alignment and cache locality.
 */
template<typename T, int SmallSize = 32 - sizeof(size_t)>
class SmallVector {
public:
    SmallVector() {
    }
    SmallVector(const T* pData, size_t size) {
        allocateAndSet(pData, size);
    }
    SmallVector(const SmallVector& other) :
                    SmallVector(other.addressof(), other.m_Size) {
    }
    SmallVector(SmallVector&& tmp) {
        memcpy(this, &tmp, sizeof(SmallVector<T>));
        tmp.m_Size = 0;
    }
    SmallVector(std::initializer_list<T> il) :
                    SmallVector(il.begin(), il.size()) {
    }
    ~SmallVector() {
        deallocateIfNeeded();
    }

    SmallVector& operator=(const SmallVector& other) {
        allocateAndSet(other.addressof(), other.m_Size);
        return *this;
    }

    bool operator==(const SmallVector& other) const {
        return m_Size == other.m_Size && memcmp(addressof(), other.addressof(), m_Size * sizeof(T)) == 0;
    }
    bool operator!=(const SmallVector& other) const {
        return !operator==(other);
    }
    bool operator<(const SmallVector& other) const {
        if(m_Size == other.m_Size )
            return memcmp(addressof(), other.addressof(), m_Size * sizeof(T))<0;
        return m_Size < other.m_Size;
    }

    size_t size() const {
        return m_Size;
    }
    const T* data() const {
        return addressof();
    }
    const T* begin() const {
        return addressof();
    }
    const T* end() const {
        return addressof() + m_Size;
    }
    enum { kSmallSize = SmallSize };

    size_t elementSize() const { return m_Size * sizeof(T); }

    bool isAllocated() const { return elementSize() > kSmallSize; }
private:
    static_assert(kSmallSize>=sizeof(void*), "Small data should be pointer size at least");

    size_t m_Size = 0;
    union {
        char m_SmallData[kSmallSize];
        T* ptr;
    };

    const T* addressof() const { return isAllocated() ? ptr : &m_SmallData[0]; }
    T* addressof() { return isAllocated() ? ptr : &m_SmallData[0]; }

    void allocateAndSet(const T* pData, size_t size) {
        deallocateIfNeeded();
        m_Size = size;
        if (isAllocated())
            ptr = new T[size];
        memcpy(addressof(), pData, elementSize());
    }

    void deallocateIfNeeded() {
        if (isAllocated())
            delete[] ptr;
    }
};
