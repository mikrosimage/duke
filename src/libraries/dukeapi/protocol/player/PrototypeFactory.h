#ifndef PrototypeFactory_H_
#define PrototypeFactory_H_

#include "protocol.pb.h"

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/ptr_container/ptr_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

std::string& replaceAll(std::string& context, const std::string& from, const std::string& to);

class PrototypeFactory : boost::noncopyable {
    typedef duke::protocol::FunctionPrototype Function;
    typedef boost::ptr_map<std::string, Function> TMap;
    TMap m_Map;
public:
    PrototypeFactory(const char* directory) {
        using namespace ::boost::filesystem;
        using namespace ::std;
        const path dir_path(directory);
#ifdef DEBUG
        cout << "[PrototypeFactory] reading from path \""<< dir_path << '"' << endl;
#endif
        directory_iterator end_itr; // default construction yields past-the-end
        for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {
            if (itr->path().filename().string()[0] == '.')
                continue;
            Function proto;
            ::boost::filesystem::ifstream infile(*itr, std::ios::binary);
            ::google::protobuf::io::IstreamInputStream zcis(&infile);
            ::google::protobuf::TextFormat::Parse(&zcis, &proto);
            proto.CheckInitialized();
            setPrototype(proto);
        }
    }

    void setPrototype(const Function& proto) {
        if (proto.signature().has_name() == false)
            return;
        setPrototype(proto.signature().name(), proto);
    }

    void setPrototype(const std::string& name, const Function& proto) {
#ifdef DEBUG
        std::cout << "[PrototypeFactory] add " << name << std::endl;
#endif
        std::auto_ptr<Function> pProto(proto.New());
        pProto->CopyFrom(proto);
        m_Map.insert(name, pProto);
    }

    const Function& getPrototype(const std::string& name) const {
        TMap::const_iterator itr = m_Map.find(name);
        if (itr == m_Map.end())
            throw std::runtime_error("Prototype " + name + " is not available");
        return *(itr->second);
    }
};

#endif /* PrototypeFactory_H_ */
