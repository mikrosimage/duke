/*
 * ImageDecoderFactoryImpl.h
 *
 *  Created on: 30 sept. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef IMAGEDECODERFACTORYIMPL_H_
#define IMAGEDECODERFACTORYIMPL_H_

#include "PluginInstance.h"

#include <duke_io/ImageDecoderFactory.h>

#include <openfx/support/host/PluginManager.h>
#include <openfx/support/host/HostImpl.h>

#include <boost/shared_ptr.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/unordered_map.hpp>

#include <string>

class ImageDecoderFactoryImpl : public ::openfx::host::HostImpl{
    ::openfx::host::PluginManager m_PluginManager;

    struct iequal_to : std::binary_function<std::string, std::string, bool> {
        bool operator()(std::string const& x, std::string const& y) const {
            return boost::algorithm::iequals(x, y, std::locale());
        }
    };

    struct ihash : std::unary_function<std::string, std::size_t> {
        std::size_t operator()(std::string const& x) const {
            std::size_t seed = 0;
            std::locale locale;
            for (std::string::const_iterator it = x.begin(); it != x.end(); ++it)
                boost::hash_combine(seed, std::toupper(*it, locale));
            return seed;
        }
    };
    // hash map
    typedef boost::shared_ptr<PluginInstance> PluginInstancePtr;
    typedef boost::unordered_map<std::string, PluginInstancePtr, ihash, iequal_to> ExtensionToDecoderMap;
    ExtensionToDecoderMap m_Map;

public:
    ImageDecoderFactoryImpl();
    virtual ~ImageDecoderFactoryImpl();

    FormatHandle getImageDecoder(const char* extension, bool &delegateRead, bool &isFormatUncompressed) const;
    bool readImageHeader(const char* filename, FormatHandle decoder, ImageDescription& description) const;
    bool decodeImage(FormatHandle decoder, const ImageDescription& description) const;
};

#endif /* IMAGEDECODERFACTORYIMPL_H_ */
