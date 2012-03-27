/*
 * CmdLineParser.h
 *
 *  Created on: 27 mars 2012
 *      Author: Guillaume Chatelet
 */

#ifndef CMDLINEPARSER_H_
#define CMDLINEPARSER_H_

#include <dukeapi/extension_set.hpp>
#include <playlist.pb.h>
#include <vector>
#include <string>
#include <stdexcept>

struct cmdline_exception : public std::invalid_argument {
    cmdline_exception(const std::string &msg) : invalid_argument(msg) { }
};

duke::playlist::Playlist browsePlayer(const extension_set &valid_extensions, const std::vector<std::string> &inputs);
duke::playlist::Playlist browseViewer(const extension_set &valid_extensions, const std::string &input);
duke::playlist::Playlist browseViewerComplete(const extension_set &valid_extensions, const std::string &input);

#endif /* CMDLINEPARSER_H_ */
