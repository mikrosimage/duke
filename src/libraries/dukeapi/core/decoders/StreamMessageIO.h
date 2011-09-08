/*
 * StreamMessageIO.h
 *
 *  Created on: 28 juin 2010
 *      Author: Nicolas Rondaud
 */

#ifndef STREAMMESSAGEIO_H_
#define STREAMMESSAGEIO_H_

#include <dukeapi/core/IMessageIO.h>
#include <dukeapi/core/queue/MessageQueue.h>

#include <boost/asio.hpp>

#include <boost/thread/thread.hpp>

class StreamMessageIO : public IMessageIO {
public:
    StreamMessageIO(std::iostream &stream);
    virtual ~StreamMessageIO();

    virtual void push(const SharedMessage& holder);
    virtual void waitPop( SharedMessage& holder );
    virtual bool tryPop( SharedMessage& holder );

private:
    void receiveLoop();
    void sendLoop();

    std::iostream  &m_Stream;
    MessageQueue m_SendQueue;
    MessageQueue m_ReceiveQueue;
    boost::thread m_SendThread;
    boost::thread m_ReceiveThread;
};

#endif /* STREAMMESSAGEIO_H_ */
