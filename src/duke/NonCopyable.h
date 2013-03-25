/*
 * NonCopyable.h
 *
 *  Created on: Dec 4, 2012
 *      Author: Guillaume Chatelet
 */

#pragma once

struct noncopyable {
    noncopyable() = default;
    noncopyable(const noncopyable&) = delete;
    noncopyable & operator=(const noncopyable&) = delete;
};
