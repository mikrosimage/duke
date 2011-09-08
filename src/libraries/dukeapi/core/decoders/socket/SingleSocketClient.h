#ifndef SINGLESOCKETCLIENT_H_
#define SINGLESOCKETCLIENT_H_

#include <boost/asio.hpp>

class SingleSocketClient {
    boost::asio::ip::tcp::iostream m_Stream;
public:
    SingleSocketClient(const std::string ip, const short port);
    virtual ~SingleSocketClient();

    boost::asio::ip::tcp::iostream& getStream() {
        return m_Stream;
    }
};

#endif /* SINGLESOCKETCLIENT_H_ */
