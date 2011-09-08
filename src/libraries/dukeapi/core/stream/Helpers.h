/*
 * Helpers.h
 *
 *  Created on: Dec 12, 2010
 *      Author: Guillaume Chatelet
 */

#ifndef HELPERS_H_
#define HELPERS_H_

#include <dukeapi/core/MessageHolder.h>

#include <iostream>

enum EIOState {
    IO_SUCCESS_OK, IO_FAILED_EOF, IO_FAILED_CORRUPTED_STREAM, IO_BAD_MESSAGE, IO_FATAL_ERROR
};

/**
 * reads one message from the stream and populate the message holder accordingly
 */
EIOState readHolder(SharedMessage &holder, std::istream& fromStream) throw ();

/**
 * write the message holder to the stream
 */
EIOState writeHolder(const SharedMessage &holder, std::ostream& toStream) throw ();

#endif /* HELPERS_H_ */
