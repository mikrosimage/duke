/*
 * FsUtils.h
 *
 *  Created on: Jan 6, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef FSUTILS_H_
#define FSUTILS_H_

#include <string>

namespace duke {

enum class FileStatus {
	NOT_A_FILE = 0, FILE = 1, DIRECTORY = 2
};

FileStatus getFileStatus(const char* filename);

const char* fileExtension(const char* pFilename);

std::string getAbsoluteFilename(const char* pFilename);

std::string getExePath();

std::string getDirname(const std::string &file);

} /* namespace duke */
#endif /* FSUTILS_H_ */
