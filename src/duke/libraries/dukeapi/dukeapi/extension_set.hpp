/*
 * extension_set.hpp
 *
 *  Created on: 26 mars 2012
 *      Author: Guillaume Chatelet
 */

#ifndef EXTENSION_SET_HPP_
#define EXTENSION_SET_HPP_

#include <boost/algorithm/string.hpp>
#include <set>
#include <string>

namespace details {
struct ci_less : std::binary_function<std::string, std::string, bool> {
    inline bool operator()(const std::string & s1, const std::string & s2) const {
        return boost::algorithm::ilexicographical_compare(s1, s2);
    }
};
} // namespace details

struct extension_set : public std::set<std::string, details::ci_less> {
    bool match(const std::string &filename) const {
        const size_t dotPos = filename.find_last_of('.');
        if (dotPos == std::string::npos)
            return false;
        return find(filename.substr(dotPos)) != end();
    }

    static extension_set create(const char **pNullTerminatedExtList) {
        extension_set result;
        for (; pNullTerminatedExtList != NULL && *pNullTerminatedExtList != NULL; ++pNullTerminatedExtList) {
            std::string extension = *pNullTerminatedExtList;
            if (!extension.empty() && extension[0] != '.')
                extension.insert(extension.begin(), '.');
            result.insert(extension);
        }
        return result;
    }
};

#endif /* EXTENSION_SET_HPP_ */
