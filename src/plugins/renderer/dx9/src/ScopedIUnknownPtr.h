#ifndef SCOPEDIUNKNOWNPTR_H_
#define SCOPEDIUNKNOWNPTR_H_

#include <Unknwn.h>
#include <cassert>
#include <iostream>

template<typename IUNKNOWN>
class ScopedIUnknownPtr {
    IUNKNOWN* m_Ptr;
    const char* m_Name;

public:
    ScopedIUnknownPtr(IUNKNOWN* ptr, const char* name = NULL) :
        m_Ptr(ptr), m_Name(name) {
    }

    ScopedIUnknownPtr(const char* name = NULL) :
        m_Ptr(NULL), m_Name(name) {
    }

    ~ScopedIUnknownPtr() {
        release();
    }

    void reset(IUNKNOWN* ptr) {
        if (ptr == m_Ptr)
            return;
        release();
        m_Ptr = ptr;
    }

    operator IUNKNOWN *() const {
        assert( m_Ptr );
        return m_Ptr;
    }

    IUNKNOWN** operator &() {
        assert( m_Ptr );
        return &m_Ptr;
    }

    IUNKNOWN* operator ->() {
        assert( m_Ptr );
        return m_Ptr;
    }

    IUNKNOWN* operator ->() const {
        assert( m_Ptr );
        return m_Ptr;
    }

    void release() {
        if (m_Ptr) {
            m_Ptr->Release();
            m_Ptr = NULL;
            if (m_Name)
                std::cerr << "releasing " << m_Name << std::endl;
        }
    }

};

#endif /* SCOPEDIUNKNOWNPTR_H_ */
