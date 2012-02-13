/*
 * DukeClient.h
 *
 *  Created on: 19 sept. 2011
 *      Author: Guillaume Chatelet
 */

#ifndef DUKECLIENT_H_
#define DUKECLIENT_H_

#include "ProtobufSerialize.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace boost {
namespace asio {

typedef buffers_iterator<streambuf::const_buffers_type> asio_buffer_itr;

/**
 * this function is used by boost::asio::read_until to read a varint
 */
std::pair<asio_buffer_itr, bool> match_varint(asio_buffer_itr begin, asio_buffer_itr end) {
    asio_buffer_itr i = begin;
    while (i != end)
        if (((uint8_t) *i++) < 0x80)
            return std::make_pair(i, true);
    return std::make_pair(i, false);
}

} // namespace asio
} // namespace boost

namespace google {
namespace protobuf {
namespace serialize {

namespace ba = boost::asio;

// forward declaration
struct ISession;

typedef boost::shared_ptr<ISession> duke_session_ptr;

typedef boost::function<ISession*(ba::io_service&)> connectionCreatorFunction;

struct ISession {
    virtual void start()=0;
    virtual ba::ip::tcp::socket& socket()=0;

    virtual ~ISession() {
    }
};

class DukeSession : public ISession, public boost::enable_shared_from_this<DukeSession> {
public:
    DukeSession(ba::io_service& io_service) :
        socket_(io_service) {
    }

    virtual ~DukeSession() {
        close();
    }

    virtual void start() {
        incomingConnection();
        read_header();
    }

    ba::ip::tcp::socket& socket() {
        return socket_;
    }

protected:
    virtual void close() {
        socket_.close();
    }

    virtual void post(const MessageHolder &holder) {
        writeDelimitedTo(post_msg_buffer_, holder);
        ba::write(socket_, ba::buffer(post_msg_buffer_));
    }

    virtual void packAndPost(const google::protobuf::Message &msg) {
        pack(post_msg_holder_, msg);
        post(post_msg_holder_);
    }

    virtual void incomingConnection() {
    }

    virtual void incomingMessage(const MessageHolder &msg) {
    }

    virtual void disconnecting(const boost::system::error_code&) {
    }

private:
    inline void read_header() {
        ba::async_read_until(socket_, headerbuffer_, ba::match_varint, boost::bind(&DukeSession::handle_read_header, this, ba::placeholders::error, _2));
    }

    inline void read_additionnal_data(void* pData, size_t size) {
        using namespace boost::asio;
        using boost::bind;
        async_read(socket_, ba::buffer(pData, size), transfer_all(), bind(&DukeSession::read_message_handler, this, ba::placeholders::error));
    }

    inline const boost::uint8_t * headerBufferPtr() const {
        return ba::buffer_cast<const boost::uint8_t*>(headerbuffer_.data());
    }

    void handle_read_header(const boost::system::error_code& error, std::size_t headerSize) {
        if (!error) {
            google::protobuf::io::CodedInputStream reader(headerBufferPtr(), headerSize);
            uint32_t messageSize = 0;
            reader.ReadVarint32(&messageSize);
            headerbuffer_.consume(headerSize);

            const uint32_t availableData = headerbuffer_.size();
            const uint32_t byteToTransfer = std::min(availableData, messageSize);
            rcv_msg_buffer_.resize(messageSize);
            std::copy(headerBufferPtr(), headerBufferPtr() + byteToTransfer, rcv_msg_buffer_.begin());
            headerbuffer_.consume(byteToTransfer);

            if (byteToTransfer == messageSize) { // message already filled
                read_message_handler(error);
            } else { // waiting for additional data
                const size_t neededSize = messageSize - byteToTransfer;
                void* pData = rcv_msg_buffer_.data() + byteToTransfer;
                read_additionnal_data(pData, neededSize);
            }
        } else {
            disconnecting(error);
        }
    }

    void read_message_handler(const boost::system::error_code &error) {
        if (!error) {
            rcv_msg_holder_.Clear();
            rcv_msg_holder_.ParseFromArray(rcv_msg_buffer_.data(), rcv_msg_buffer_.size());
            incomingMessage(rcv_msg_holder_);
            read_header();
        } else {
            disconnecting(error);
        }
    }

    ba::ip::tcp::socket socket_;
    ba::streambuf headerbuffer_;
    raw_buffer rcv_msg_buffer_;
    MessageHolder rcv_msg_holder_;
    raw_buffer post_msg_buffer_;
    MessageHolder post_msg_holder_;
};

typedef boost::shared_ptr<DukeSession> duke_session_impl_ptr;

struct duke_common {
    duke_common(const connectionCreatorFunction& cc) :
        connection_creator_(cc) {
    }

    virtual ~duke_common() {
    }

    inline const boost::system::error_code &error() const throw () {
        return error_code_;
    }

    void run() {
        io_service_.run();
        if (error())
            cerr << error().message() << endl;
    }

protected:
    void incomingConnection(duke_session_ptr pSession) {
        current_connection_ = pSession;
        current_connection_->start();
    }

    duke_session_ptr createConnection() {
        return duke_session_ptr(connection_creator_(io_service_));
    }

    ba::io_service io_service_;
    boost::system::error_code error_code_;
private:
    duke_session_ptr current_connection_;
    connectionCreatorFunction connection_creator_;
};

struct duke_server : public duke_common {
    duke_server(const ba::ip::tcp::endpoint& endpoint, const connectionCreatorFunction&cc) :
        duke_common(cc), acceptor_(io_service_, endpoint) {
        start_accept();
    }

private:
    void start_accept() {
        duke_session_ptr pSession(createConnection());
        acceptor_.async_accept(pSession->socket(), boost::bind(&duke_server::handle_accept, this, pSession, ba::placeholders::error));
    }

    void handle_accept(duke_session_ptr pSession, const boost::system::error_code& error) {
        error_code_ = error;
        if (!error)
            incomingConnection(pSession);
    }

    ba::ip::tcp::acceptor acceptor_;
};

struct duke_client : public duke_common {
    duke_client(const ba::ip::tcp::resolver::query &query, const connectionCreatorFunction&cc) :
        duke_common(cc), resolver_(io_service_) {
        resolver_.async_resolve(query, boost::bind(&duke_client::resolve_handler, this, ba::placeholders::error, _2));
    }

private:
    void resolve_handler(const boost::system::error_code &error, ba::ip::tcp::resolver::iterator it) {
        error_code_ = error;
        if (!error) {
            duke_session_ptr pSession(createConnection());
            pSession->socket().async_connect(*it, boost::bind(&duke_client::connect_handler, this, pSession, ba::placeholders::error));
        }
    }

    void connect_handler(duke_session_ptr pSession, const boost::system::error_code &error) {
        error_code_ = error;
        if (!error)
            incomingConnection(pSession);
    }

    ba::ip::tcp::resolver resolver_;
};

} // namespace serialize
} // namespace protobuf
} // namespace google


#endif /* DUKECLIENT_H_ */
