/*
 * Version.h
 *
 *  Created on: 6 mars 2012
 *      Author: Guillaume Chatelet
 */

#ifndef VERSION_H_
#define VERSION_H_

#include <string>

#ifdef GIT_COMMIT
static const char * const gDukeHash = GIT_COMMIT;
#else
static const char * const gDukeHash = "NA";
#endif

#ifdef GIT_BRANCH
static const char * const gDukeVersion = GIT_BRANCH;
#else
static const char * const gDukeVersion = "dev";
#endif

#ifdef JAMDATE
static const char * const gCompileDate = JAMDATE;
#else
static const char * const gCompileDate = "compilation date unavailable";
#endif

#ifdef DEBUG
static const char * const gVariant = " - DEBUG";
#else
static const char * const gVariant = "";
#endif

static inline std::string getVersion(const char * applicationName) {
    std::string s(applicationName);
    s += ' ';
    s += gDukeVersion;
    s += gVariant;
    s += '\n';
    s += gCompileDate;
    s += " #";
    s += gDukeHash;
    return s;
}

#endif /* VERSION_H_ */
