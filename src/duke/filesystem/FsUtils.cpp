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
#include <limits.h>
#include <unistd.h>

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
	char result[PATH_MAX];
	if (!realpath(pFilename, result))
		return {};
	return result;
}

// from http://www.cplusplus.com/forum/general/11104/
std::string getExePath() {
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}

std::string getDirname(const std::string &file) {
	return file.substr(0, file.rfind('/'));
}

} /* namespace duke */
