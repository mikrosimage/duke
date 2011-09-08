/*
 * ResourceManager.cpp
 *
 *  Created on: 19 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "ResourceManager.h"
#include "IResource.h"
#include <string>
#include <vector>
#include <cassert>
#include <iostream>

ResourceManager::ResourceManager() {
}

ResourceManager::~ResourceManager() {
#ifdef DEBUG
    std::cout << "ResourceManager : freeing resources\n";
    for (TTypeMap::const_iterator itr = m_TypeMap.begin(); itr != m_TypeMap.end(); ++itr) {
        std::cout << "Releasing type : " << resource::TypeToString( itr->first ) << std::endl;
        const TResourcesMap& map = itr->second;
        for (TResourcesMap::const_iterator i = map.begin(); i != map.end(); ++i) {
            const IResource* pResource = i->second.get();
            std::cout << " - \"" << pResource->getName();
            std::cout << "\" ( " << pResource->getSize() << " B )" << std::endl;
        }
    }
#endif
}

void ResourceManager::add(const std::string& name, const ResourcePtr& resource, bool checkResourceAlreadyStored) {
    assert( !name.empty() );
    assert( resource != NULL );
    resource->m_sName = name;

    TResourcesMap& map = m_TypeMap[resource->getResourceType()];
    if (checkResourceAlreadyStored && map.find(name) != map.end())
        std::cout << name << " : resource already loaded !" << std::endl;

    map[name] = resource;
}

void ResourceManager::remove(const ::resource::Type type, const std::string& name) {
    TResourcesMap& map = m_TypeMap[type];

    if (type == ::resource::SHADER) {
        typedef std::vector<std::string> STRINGS;
        STRINGS names;
        for (TResourcesMap::const_iterator itr = map.begin(); itr != map.end(); ++itr) {
            const std::string &mapName = itr->first;
            // erasing shaders with the same name or with a tweaked name
            // not a very good design as this part is now dependent of the ShaderFactory::getTweakedName()
            if (mapName == name || mapName.compare(0, name.size() + 1, name + '|') == 0)
                names.push_back(mapName);
        }
        for (STRINGS::const_iterator itr = names.begin(); itr != names.end(); ++itr)
            map.erase(*itr);
    } else {
        map.erase(name);
    }
}
