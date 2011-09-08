/*
 * StreamMessageIO.cpp
 *
 *  Created on: 28 juin 2010
 *      Author: Nicolas Rondaud
 */

#include "StreamMessageIO.h"
#include <dukeapi/core/stream/Helpers.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/locks.hpp>
#include <iostream>
#include <stdexcept>

#include <google/protobuf/descriptor.h>

using namespace boost::asio::ip;
using namespace protocol::duke;
using namespace std;

const string HEADER = "[SocketMessageIO] ";
const string RECEIVEHEADER = HEADER + "-receive thread- ";
const string SENDHEADER = HEADER + "-send thread- ";

StreamMessageIO::StreamMessageIO(std::iostream &stream) :
    m_Stream(stream),//
            m_SendThread(&StreamMessageIO::sendLoop, this), //
            m_ReceiveThread(&StreamMessageIO::receiveLoop, this) {
}

StreamMessageIO::~StreamMessageIO() {
    m_ReceiveThread.join();
    m_SendThread.join();
}

const char * getStopMessage(EIOState state) {
    switch (state) {
        case EIOState::IO_FAILED_EOF:
            return "Connection reset by peer. stopping.";
        case EIOState::IO_FAILED_CORRUPTED_STREAM:
            return "Stream corrupted. stopping.";
        case EIOState::IO_FATAL_ERROR:
            return "Fatal error. stopping.";
        default:
            return NULL;
    }
}

void StreamMessageIO::receiveLoop() {
    try {
        cout << RECEIVEHEADER + "starting." << endl;
        SharedMessage holder;
        const char* stopMessage = NULL;
        int retCode = 0;
        while (stopMessage == NULL) {
            EIOState ioState = readHolder(holder, m_Stream);
            if (ioState == EIOState::IO_SUCCESS_OK) {
                if (holder->type() == MessageType_Type_QUIT){
                    ioState = EIOState::IO_FAILED_EOF;
                    const Quit& q = holder->message<Quit>();
                    retCode = q.returncode();
                }
                else
                    m_ReceiveQueue.push(holder);
            }
            stopMessage = getStopMessage(ioState);
        }
        Quit quit;
        if (stopMessage != NULL) {
            cerr << RECEIVEHEADER + stopMessage << endl;
            quit.add_quitmessage(stopMessage);
            quit.set_returncode(retCode);
        }
        ::push(m_ReceiveQueue, quit);
    } catch (exception &e) {
        cerr << "exception occurred in SocketMessageIO::receiveLoop : " << e.what() << endl;
    }
    // pushing a null message so sendLoop will also stop
    m_SendQueue.push(SharedMessage());
}

void StreamMessageIO::sendLoop() {
    try {
        cout << SENDHEADER + "starting." << endl;
        SharedMessage holder;
        const char* stopMessage = NULL;
        while (stopMessage == NULL) {
            m_SendQueue.waitPop(holder);
            if (!holder) {
                cout << SENDHEADER + "received sentinel. stopping." << endl;
                break;
            }
            const EIOState ioState = writeHolder(holder, m_Stream);
            if (ioState == EIOState::IO_SUCCESS_OK)
                m_Stream.flush();
            stopMessage = getStopMessage(ioState);
        }
        if (stopMessage != NULL)
            cerr << SENDHEADER + stopMessage << endl;
    } catch (exception &e) {
        cerr << "exception occurred in SocketMessageIO::sendLoop : " << e.what() << endl;
    }
}

bool StreamMessageIO::tryPop(SharedMessage& holder) {
    return m_ReceiveQueue.tryPop(holder);
}

void StreamMessageIO::waitPop(SharedMessage& holder) {
    m_ReceiveQueue.waitPop(holder);
}

void StreamMessageIO::push(const SharedMessage& holder) {
    if (!holder)
        throw runtime_error("Trying to send a NULL Message");
    m_SendQueue.push(holder);
}
