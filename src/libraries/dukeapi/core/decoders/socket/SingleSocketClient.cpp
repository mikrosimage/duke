/*
 * SingleSocketClient.h
 *
 *  Created on: March 28, 2011
 *      Author: Rondaud Nicolas
 */

#include "SingleSocketClient.h"
#include <string>
#include <iostream>
#include <boost/timer.hpp>
#include <boost/thread.hpp>

using namespace boost::asio::ip;
using namespace std;

const string HEADER = "[SingleSocketClient] ";

SingleSocketClient::SingleSocketClient(const std::string ip, const short port) :
    m_Stream(tcp::endpoint(address::from_string(ip), port)) {

    size_t i = 0, attempt = 5;
    while(i < attempt){
        if(m_Stream.good()){
           cout << HEADER << "connected to server" << endl;
           return;
        }
        i++;
        cout << HEADER << "waiting..." << endl;
        boost::this_thread::sleep(boost::posix_time::millisec(60));
    }
    throw runtime_error(HEADER + "Error while connecting to server.");
}

SingleSocketClient::~SingleSocketClient() {
    m_Stream.close();
}
