/*
 * FsUtils.cpp
 *
 *  Created on: Jan 6, 2013
 *      Author: Guillaume Chatelet
 */

#include "FsUtils.h"

#include <dirent.h>
#include <sys/stat.h>

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

} /* namespace duke */
