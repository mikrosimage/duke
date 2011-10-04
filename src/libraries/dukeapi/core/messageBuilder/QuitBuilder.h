/*
 * QuitBuilder.h
 *
 *  Created on: 4 oct. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef QUITBUILDER_H_
#define QUITBUILDER_H_

#include <cstdlib> // for EXIT_SUCCESS and EXIT_FAILURE
#define EXIT_RELAUNCH -1

namespace google {
namespace protobuf {
namespace serialize {

inline MessageHolder close_connection(int32_t exit_code) {
    MessageHolder holder;
    holder.set_action(MessageHolder_Action_CLOSE_CONNECTION);
    holder.set_return_value(exit_code);
    return holder;
}

inline MessageHolder quitFailure() {
    return close_connection(EXIT_FAILURE);
}

inline MessageHolder quitSuccess() {
    return close_connection(EXIT_SUCCESS);
}

inline MessageHolder relaunch() {
    return close_connection(EXIT_RELAUNCH);
}

} // namespace serialize
} // namespace protobuf
} // namespace google


#endif /* QUITBUILDER_H_ */
