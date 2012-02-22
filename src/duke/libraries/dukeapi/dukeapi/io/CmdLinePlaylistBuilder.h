/*
 * CmdLinePlaylistBuilder.h
 *
 *  Created on: 22 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#ifndef CMDLINEPLAYLISTBUILDER_H_
#define CMDLINEPLAYLISTBUILDER_H_

#include <dukeapi/sequence/PlaylistBuilder.h>

#include <boost/noncopyable.hpp>

#include <memory>

struct CmdLinePlaylistBuilder : private boost::noncopyable {
    struct Pimpl;
    CmdLinePlaylistBuilder(bool useContainingSequence, const char **validExtensions);
    void operator()(const std::string& entry);
    struct Proxy{
        Proxy(CmdLinePlaylistBuilder *ptr) : ptr(ptr){}
        inline void operator()(const std::string& entry){ (*ptr)(entry); }
        CmdLinePlaylistBuilder *ptr;
    };
    inline Proxy functor(){ return Proxy(this); }
private:
    std::auto_ptr<Pimpl> m_Pimpl;
};

#endif /* CMDLINEPLAYLISTBUILDER_H_ */
