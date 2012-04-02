/*
 * CmdLineOptions.cpp
 *
 *  Created on: 2 avr. 2012
 *      Author: Guillaume Chatelet
 */

#include <string>
#include <sstream>

#include <dukeapi/protobuf_builder/CmdLineParser.h>
#include <dukeengine/memory/MemoryUtils.h>

using namespace std;

static const unsigned long long Kilo = 1024;
static const unsigned long long Mega = Kilo * Kilo;
static const unsigned long long Giga = Kilo * Mega;

unsigned long long parseCache(const std::string& line) {
    if (line.empty())
        throw cmdline_exception("can't parse an empty cache size");
    istringstream stream(line);
    double value = 0;
    stream >> value;
    if (stream.bad())
        throw cmdline_exception(string("can't parse an cache size : ") + line);
    if (stream.eof())
        return value * Mega;
    string unit;
    stream >> unit;
    if (unit.size() != 1)
        throw cmdline_exception(string("Unknown unit for cache size : ") + line);
    switch (unit[0]) {
        case 'k':
        case 'K':
            return value * Kilo;
        case 'm':
        case 'M':
            return value * Mega;
        case 'g':
        case 'G':
            return value * Giga;
        case '%': {
            value /= 100;
            if (value > 1)
                throw cmdline_exception(string("It's not very wise to use more memory than you have : ") + line);
            return value = getTotalSystemMemory() * value;
        }
    }
    throw cmdline_exception(string("Unknown unit for cache size : ") + line);
}
