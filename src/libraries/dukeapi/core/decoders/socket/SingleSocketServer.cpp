/*
 * SingleSocketServer.cpp
 *
 *  Created on: Dec 11, 2010
 *      Author: Guillaume Chatelet
 */

#include "SingleSocketServer.h"
#include <string>
#include <iostream>

using namespace boost::asio::ip;
using namespace std;

const string HEADER = "[SingleSocketServer] ";

SingleSocketServer::SingleSocketServer(const short port) :
    m_Endpoint(tcp::v4(), port), m_Stream(m_TcpStream.rdbuf()) {
    // creating a connection acceptor
    tcp::acceptor acceptor(m_IoService, m_Endpoint, true);
    // waiting for incoming connection
    cout << HEADER << "listening on " << m_Endpoint << endl;
    acceptor.accept(*m_TcpStream.rdbuf(), m_PeerEndpoint);
    cout << HEADER << "incoming connection from " << m_PeerEndpoint << endl;
}

SingleSocketServer::~SingleSocketServer() {
    m_TcpStream.close();
}
