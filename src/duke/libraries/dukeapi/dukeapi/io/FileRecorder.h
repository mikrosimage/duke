#ifndef FILEWRITERINPUTDECORATOR_H_
#define FILEWRITERINPUTDECORATOR_H_

#include <dukeapi/IMessageIO.h>
#include <fstream>
#include <memory>

class FileRecorder : public IMessageIO {
public:
    FileRecorder(const char* filename, IMessageIO& delegate);
    virtual ~FileRecorder();

    virtual void push(const ::google::protobuf::serialize::SharedHolder& holder);
    virtual void waitPop(::google::protobuf::serialize::SharedHolder& holder);
    virtual bool tryPop(::google::protobuf::serialize::SharedHolder& holder);
private:
    IMessageIO& m_Delegate;
    std::ofstream m_Stream;
};

#endif /* FILEWRITERINPUTDECORATOR_H_ */
