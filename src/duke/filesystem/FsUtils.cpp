/*
 * FsUtils.cpp
 *
 *  Created on: Jan 6, 2013
 *      Author: Guillaume Chatelet
 */

#include "FsUtils.h"

#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

namespace duke {

FileStatus getFileStatus(const char* filename) {
	struct stat statbuf;
	if (stat(filename, &statbuf) == -1)
		return FileStatus::NOT_A_FILE;
	if (S_ISREG(statbuf.st_mode))
		return FileStatus::FILE;
	if (S_ISDIR(statbuf.st_mode))
		return FileStatus::DIRECTORY;
	return FileStatus::NOT_A_FILE;
}

const char* fileExtension(const char* pFilename) {
	const char* pDot = strrchr(pFilename, '.');
	if (!pDot)
		return nullptr;
	return ++pDot;
}

std::string getAbsoluteFilename(const char* pFilename) {
	std::string toAbsolute;
	toAbsolute.resize(PATH_MAX);
	if (!realpath(pFilename, const_cast<char*>(toAbsolute.data())))
		toAbsolute.clear();
	toAbsolute.resize(strlen(toAbsolute.c_str()));
	return toAbsolute;
}

} /* namespace duke */
