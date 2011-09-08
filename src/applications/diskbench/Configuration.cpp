/*
 * Configuration.cpp
 *
 *  Created on: 15 avr. 2009
 *      Author: Guillaume Chatelet
 */

#include "Configuration.h"
#ifdef _WIN32
    #include <dukeengine/file/DmaFileIO.h>
#endif
#include <dukeengine/file/StreamedFileIO.h>
#include <dukeengine/memory/alloc/Allocators.h>
//#include <dukeengine/time_statistics/StopWatch.h>
#include "FileGrabber.h"
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <functional>

namespace po = boost::program_options;
namespace pt = ::boost::posix_time;
using namespace std;

const size_t ONE_TIME_ALLOC_SIZE = 20 * 1024 * 1024;
struct OneTimeBigBlockAllocated : public ::mikrosimage::alloc::Allocator
{
	typedef ::mikrosimage::alloc::AlignedMalloc ALLOC;
	char* pData;
	OneTimeBigBlockAllocated()
		: pData( ALLOC::malloc( ONE_TIME_ALLOC_SIZE ) ) {}

	~OneTimeBigBlockAllocated()
	{
		ALLOC::free( pData );
	}

	virtual char* malloc( const size_t size )
	{
		return pData;
	}

	virtual void free( char* const ptr ) {}

	virtual size_t alignment() const
	{
		return ALLOC::aligned;
	}
};

enum AllocMethod
{
	ALIGNED_MALLOC, MALLOC, NEW, VIRTUAL, REUSE, LAST_ALLOC
};

enum ReadMethod
{
	STREAMED, MAPPED, LAST_READ_METHOD
};

AllocMethod getAllocEnum( const string& value )
{
	if( value == "malloc" )
		return MALLOC;
	if( value == "aligned_malloc" )
		return ALIGNED_MALLOC;
	if( value == "new" )
		return NEW;
	if( value == "virtual" )
		return VIRTUAL;
	if( value == "reuse" )
		return REUSE;
	throw runtime_error( "invalid alloc method " + value );
}

string toString( const AllocMethod& value )
{
	switch( value )
	{
		case ALIGNED_MALLOC:
			return "aligned_malloc";
		case MALLOC:
			return "malloc";
		case NEW:
			return "new";
		case VIRTUAL:
			return "virtual";
		case REUSE:
			return "reuse";
		default:
			throw runtime_error( "invalid RawAlloc " + value );
	}
}

string toString( const ReadMethod& value )
{
	switch( value )
	{
		case STREAMED:
			return "streamed";
		case MAPPED:
			return "mapped";
		default:
			throw runtime_error( "invalid ReadMethod " + value );
	}
}

ReadMethod getReadMethodEnum( const string& value )
{
	if( value == "streamed" )
		return STREAMED;
	if( value == "mapped" )
		return MAPPED;
	throw runtime_error( "invalid read method " + value );
}

using namespace ::mikrosimage::alloc;

AlignedMallocAllocator alignedMallocAlloc;
MallocAllocator mallocAlloc;
NewAllocator newAlloc;
VirtualAllocator virtualAlloc;
OneTimeBigBlockAllocated oneTimeBigBlockAlloc;

typedef unary_function<void, pair<boost::filesystem::path, size_t> > FUNCTION;

struct AllocateAndRead : public FUNCTION
{
	FileIO* const m_pFileIO;
	AllocateAndRead( FileIO* const pFileIO )
		: m_pFileIO( pFileIO ) {}

	void operator()( const pair<boost::filesystem::path, size_t>& pair )
	{
		m_pFileIO->read( pair.first.string().c_str() );
	}

};

void process( const string& directory, const PATH_VECTOR& paths, const AllocMethod alloc, const ReadMethod readMethod )
{
	::mikrosimage::alloc::Allocator* pAllocator = NULL;
	switch( alloc )
	{
		case ALIGNED_MALLOC:
			pAllocator = &alignedMallocAlloc;
			break;
		case MALLOC:
			pAllocator = &mallocAlloc;
			break;
		case NEW:
			pAllocator = &newAlloc;
			break;
		case VIRTUAL:
			pAllocator = &virtualAlloc;
			break;
		case REUSE:
			pAllocator = &oneTimeBigBlockAlloc;
			break;
		default:
			return;
	}

	FileIO* pFileIO = NULL;
	switch( readMethod )
	{
		case STREAMED:
			pFileIO = new StreamedFileIO( pAllocator );
			break;
			#ifdef _WIN32
		case MAPPED:
			if( pAllocator->alignment() != 4 * 1024 )
				return;
			pFileIO = new DmaFileIO( pAllocator );
			break;
			#endif
		default:
			return;
	}

	StopWatch stopWatch( true );
	for_each( paths.begin(), paths.end(), AllocateAndRead( pFileIO ) );
	::boost::posix_time::time_duration duration( stopWatch.splitTime() );

	PATH_VECTOR::const_iterator itr;
	size_t overallSizeInkB = 0;
	for( itr = paths.begin(); itr != paths.end(); ++itr )
		overallSizeInkB += itr->second / 1024;
	const double seconds         = duration.total_microseconds() / 1000000.;
	const double fileCount       = paths.size();
	const double overallSizeInMB = overallSizeInkB / 1024.;
	const double throughputInMB  = overallSizeInMB / seconds;
	const double fps             = fileCount / seconds;
	const pt::time_duration perFileDuration( duration / paths.size() );
	cout << directory << "\t";
	cout << toString( alloc ) << "\t";
	cout << toString( readMethod ) << "\t";
	cout << throughputInMB << "\t";
	cout << fps << "\t";
	cout << pt::to_simple_string( perFileDuration ) << "\t";
	cout << pt::to_simple_string( duration ) << "\t";
	cout << overallSizeInMB << "\t";
	cout << fileCount << endl;
}

Configuration::Configuration( int argc, char** argv )
	: m_Generic( "command line only options" )
{

	// available on the command line
	m_Generic.add_options() //
	    ( "help,h", "Displays this help" ) //
	    ( "version,v", "Displays the version informations" ) //
	    ( "limit,l", po::value<size_t>(), "limits the number of files to 'limit'" ) //
	    ( "directory,d", po::value<string>()->default_value( "." ), "Sets the directory to read files from, default to current directory" ) //
	    ( "full,f", "Runs all the different combinations of alloc-methods and read-method" ) //
	    ( "alloc-method", po::value<string>()->default_value( "malloc" ), "Sets the allocation method, can be 'malloc', 'aligned_malloc', 'new', 'virtual', 'reuse'" ) //
	    ( "read-method", po::value<string>()->default_value( "streamed" ), "Sets the read method, can be 'streamed' or 'mapped'" );

	// now parsing the command line
	po::store( po::parse_command_line( argc, argv, m_Generic ), m_Vm );
	po::notify( m_Vm );

	if( m_Vm.count( "help" ) )
	{
		displayHelp();
		return;
	}

	if( m_Vm.count( "version" ) )
	{
		displayVersion();
		return;
	}

	const string directory = m_Vm["directory"].as<std::string>();
	const FileGrabber fileGrabber( directory.c_str() );
	PATH_VECTOR paths( fileGrabber.getPaths() );

	if( m_Vm.count( "limit" ) )
	{
		const size_t limit = m_Vm["limit"].as<size_t>();
		if( limit < paths.size() )
			paths.resize( limit );
	}

	if( m_Vm.count( "full" ) )
	{
		for( int allocMethod = ALIGNED_MALLOC; allocMethod != LAST_ALLOC; ++allocMethod )
			for( int readMethod = STREAMED; readMethod != LAST_READ_METHOD; ++readMethod )
				process( directory, paths, AllocMethod( allocMethod ), ReadMethod( readMethod ) );
	}
	else
	{
		const AllocMethod allocMethod = getAllocEnum( m_Vm["alloc-method"].as<string>() );
		const ReadMethod readMethod   = getReadMethodEnum( m_Vm["read-method"].as<string>() );
		process( directory, paths, allocMethod, readMethod );
	}
}

void Configuration::displayVersion()
{
	cout << "Mikros Image DiskBench - (C) Copyright 2007-2010 Mikros Image";
	#if defined DEBUG
	cout << " - DEBUG";
	#else
	cout << " - RELEASE";
	#endif
	cout << endl;
}

void Configuration::displayHelp()
{
	cout << m_Generic << endl;
}

