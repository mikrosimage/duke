/*
 * ResourceManager.h
 *
 *  Created on: 19 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef RESOURCEMANAGER_H_
#define RESOURCEMANAGER_H_

#include "IResourceType.h"
#include "IResource.h"
#include "ProtoBufResource.h"

#include <boost/unordered_map.hpp>
#include <boost/intrusive_ptr.hpp>
#include <boost/functional/hash.hpp>
#include <boost/utility.hpp>
#include <string>

class ResourceManager : public ::boost::noncopyable {
public:
    ResourceManager();
    ~ResourceManager();

    template<class T>
    boost::shared_ptr<T> get(const ::resource::Type type, const std::string& name) const;
    template<class T>
    boost::shared_ptr<T> safeGet(const ::resource::Type type, const std::string& name) const;
    template<class T>
    const T& safeGetProto(const std::string& name) const;

    inline void add(const std::string& name, IResource* resource, bool checkResourceAlreadyStored = true) {
        add(name, ResourcePtr(resource), checkResourceAlreadyStored);
    }
    void add(const std::string& name, const ResourcePtr& resource, bool checkResourceAlreadyStored = true);
    void remove(const ::resource::Type type, const std::string& name);

private:
    typedef boost::unordered_map<std::string, ResourcePtr> TResourcesMap;
    typedef boost::unordered_map<resource::Type, TResourcesMap> TTypeMap;

    TTypeMap m_TypeMap;
};

template<class T>
boost::shared_ptr<T> ResourceManager::get(const ::resource::Type type, const std::string& name) const {
    const TTypeMap::const_iterator typeItr = m_TypeMap.find(type);
    if (typeItr == m_TypeMap.end())
        return boost::shared_ptr<T>();

    const TResourcesMap& map = typeItr->second;
    const TResourcesMap::const_iterator itr = map.find(name);

    if (itr == map.end())
        return boost::shared_ptr<T>();
    return boost::dynamic_pointer_cast<T>(itr->second);
}

template<class T>
boost::shared_ptr<T> ResourceManager::safeGet(const ::resource::Type type, const std::string& name) const {
    boost::shared_ptr<T> ptr = get<T> (type, name);
    if (!ptr)
        throw std::runtime_error(std::string("no registered resource with name \"") + name + "\"");
    return ptr;
}

template<class T>
const T& ResourceManager::safeGetProto(const std::string& name) const {
    return safeGet<ProtoBufResource> (::resource::PROTOBUF, name)->getRef<T> ();
}
#endif /* RESOURCEMANAGER_H_ */
