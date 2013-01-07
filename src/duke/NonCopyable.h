/*
 * NonCopyable.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Guillaume Chatelet
 */

#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

struct noncopyable {
    noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
    noncopyable & operator=(const noncopyable&) = delete;
};

#endif /* NONCOPYABLE_H_ */
