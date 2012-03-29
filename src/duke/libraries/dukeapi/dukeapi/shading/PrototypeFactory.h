#ifndef PrototypeFactory_H_
#define PrototypeFactory_H_

#include <player.pb.h>

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/noncopyable.hpp>

#include <string>

std::string& replaceAll(std::string& context, const std::string& from, const std::string& to);

struct PrototypeFactory : boost::noncopyable {
private:
    typedef boost::ptr_map<std::string, duke::protocol::FunctionPrototype> TMap;
    TMap m_Map;
public:
    void loadFromFolder(const char* directory);

    void setPrototype(const duke::protocol::FunctionPrototype& proto);
    void setPrototype(const std::string& name, const duke::protocol::FunctionPrototype& proto);

    const duke::protocol::FunctionPrototype& getPrototype(const std::string& name) const;
    bool hasPrototype(const std::string& name) const;
};

#endif /* PrototypeFactory_H_ */
