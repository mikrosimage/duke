#include "PrototypeFactory.h"

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <stdexcept>

using namespace std;
using namespace duke::protocol;
using namespace ::boost::filesystem;

string& replaceAll(string& context, const string& from, const string& to) {
    size_t lookHere = 0;
    size_t foundHere;
    while ((foundHere = context.find(from, lookHere)) != string::npos) {
        context.replace(foundHere, from.size(), to);
        lookHere = foundHere + to.size();
    }
    return context;
}

void PrototypeFactory::loadFromFolder(const char* directory) {
    const path dir_path(directory);
#ifdef DEBUG
    cout << "[PrototypeFactory] reading from path \""<< dir_path << '"' << endl;
#endif
    directory_iterator end_itr; // default construction yields past-the-end
    for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {
        if (itr->path().filename().string()[0] == '.')
            continue;
        FunctionPrototype proto;
        ::boost::filesystem::ifstream infile(*itr, std::ios::binary);
        ::google::protobuf::io::IstreamInputStream zcis(&infile);
        ::google::protobuf::TextFormat::Parse(&zcis, &proto);
        proto.CheckInitialized();
        setPrototype(proto);
    }
}

void PrototypeFactory::setPrototype(const FunctionPrototype& proto) {
    if (proto.signature().has_name() == false)
        return;
    setPrototype(proto.signature().name(), proto);
}

void PrototypeFactory::setPrototype(const std::string& name, const FunctionPrototype& proto) {
#ifdef DEBUG
    std::cout << "[PrototypeFactory] add " << name << std::endl;
#endif
    std::auto_ptr<FunctionPrototype> pProto(proto.New());
    pProto->CopyFrom(proto);
    m_Map.insert(name, pProto);
}

const FunctionPrototype& PrototypeFactory::getPrototype(const std::string& name) const {
    TMap::const_iterator itr = m_Map.find(name);
    if (itr == m_Map.end())
        throw std::runtime_error("Prototype " + name + " is not available");
    return *(itr->second);
}

bool PrototypeFactory::hasPrototype(const std::string& name) const {
    TMap::const_iterator itr = m_Map.find(name);
    return itr != m_Map.end();
}
