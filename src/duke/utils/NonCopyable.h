/*
 * NonCopyable.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable & operator=(const NonCopyable&) = delete;
};

#endif /* NONCOPYABLE_H_ */
