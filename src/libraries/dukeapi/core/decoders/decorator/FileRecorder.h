/*
 * FileRecorder.h
 *
 *  Created on: 15 avr. 2009
 *      Author: Guillaume Chatelet
 */

#ifndef FILEWRITERINPUTDECORATOR_H_
#define FILEWRITERINPUTDECORATOR_H_

#include <dukeapi/core/IMessageIO.h>
#include <fstream>
#include <memory>

class FileRecorder : public IMessageIO {
public:
    FileRecorder(const char* filename, IMessageIO& delegate);
    virtual ~FileRecorder();

    virtual void push(const SharedMessage& holder);
    virtual void waitPop(SharedMessage& holder);
    virtual bool tryPop(SharedMessage& holder);
private:
    IMessageIO& m_Delegate;
    std::ofstream m_Stream;
};

#endif /* FILEWRITERINPUTDECORATOR_H_ */
