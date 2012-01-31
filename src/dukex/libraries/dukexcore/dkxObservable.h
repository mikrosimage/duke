#ifndef DKXOBSERVABLE_H
#define DKXOBSERVABLE_H

#include "dkxIObserver.h"
#include <set>

class Observable {

public:
    inline void notify(::google::protobuf::serialize::SharedHolder holder) const {
        for (std::set<IObserver*>::const_iterator it = m_Observers.begin(); it != m_Observers.end(); ++it)
            (*it)->update(holder);
    }
    inline void addObserver(IObserver* observer) {
        m_Observers.insert(observer);
    }
    inline void removeObserver(IObserver* observer) {
        m_Observers.erase(observer);
    }

private:
    std::set<IObserver*> m_Observers;
};

#endif // DKXOBSERVABLE_H
