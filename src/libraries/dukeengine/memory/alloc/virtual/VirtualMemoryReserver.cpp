/*
 * VirtualMemoryReserver.cpp
 *
 *  Created on: 16 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifdef _WIN32

#include "VirtualMemoryReserver.h"
#include <dukeengine/memory/MemoryUtils.h>
#include <stdexcept>
#include <string>
#include <sstream>
#include <windows.h>

VirtualMemoryReserver::VirtualMemoryReserver( const size_t size )
{
	SYSTEM_INFO SystemInfo;

	GetSystemInfo( &SystemInfo );
	m_uPageSize     = SystemInfo.dwPageSize;
	m_uReservedSize = roundUp<size_t>( size, m_uPageSize );

	HANDLE hProcess = GetCurrentProcess();
	SIZE_T dwMin, dwMax;

	if( !GetProcessWorkingSetSize( hProcess, &dwMin, &dwMax ) )
	{
		std::ostringstream msg;
		msg << "GetProcessWorkingSetSize failed : ";
		msg << (long)GetLastError();
		throw std::runtime_error( msg.str() );
	}

	dwMin = std::max( (size_t)dwMin, m_uReservedSize );
	dwMax = std::max( (size_t)dwMax, m_uReservedSize );
	if( !SetProcessWorkingSetSize( hProcess, dwMin, dwMax ) )
	{
		std::ostringstream msg;
		msg << "SetProcessWorkingSetSize failed : ";
		msg << (long)GetLastError();
		throw std::runtime_error( msg.str() );
	}
}

VirtualMemoryReserver::~VirtualMemoryReserver() {}

#endif
