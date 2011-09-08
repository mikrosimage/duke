/*
 * ProtobufFactory.cpp
 *
 *  Created on: 29 oct. 2010
 *      Author: Guillaume Chatelet
 */

#include "PrototypeFactory.h"

using namespace std;
using namespace protocol::shader_assembler;

string& replaceAll(string& context, const string& from, const string& to) {
    size_t lookHere = 0;
    size_t foundHere;
    while ((foundHere = context.find(from, lookHere)) != string::npos) {
        context.replace(foundHere, from.size(), to);
        lookHere = foundHere + to.size();
    }
    return context;
}
