#ifndef SingleSocketServer_H_
#define SingleSocketServer_H_

#include <boost/asio.hpp>

class SingleSocketServer {
    boost::asio::io_service m_IoService;
    boost::asio::ip::tcp::endpoint m_Endpoint;
    boost::asio::ip::tcp::endpoint m_PeerEndpoint;
    boost::asio::streambuf m_Buffer;
    boost::asio::ip::tcp::iostream m_TcpStream;
    std::iostream m_Stream;
public:
    SingleSocketServer(const short port);
    virtual ~SingleSocketServer();

    const boost::asio::ip::tcp::endpoint& getPeerEndpoint() const {
        return m_PeerEndpoint;
    }

    std::iostream& getStream() {
        return m_Stream;
    }
};

#endif /* SINGLECONNECTIONACCEPTOR_H_ */
