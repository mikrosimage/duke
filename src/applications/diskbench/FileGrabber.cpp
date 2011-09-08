/*
 * FileGrabber.cpp
 *
 *  Created on: 19 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include "FileGrabber.h"

using namespace ::boost::filesystem;
using namespace ::std;

FileGrabber::FileGrabber( const char* dir_path )
{
	if( !exists( dir_path ) )
		throw std::runtime_error( std::string( "Invalid diretory : " )
		                          + dir_path );
	directory_iterator end_itr; // default construction yields past-the-end
	for( directory_iterator itr( dir_path ); itr != end_itr; ++itr )
		if( is_regular_file( itr->status() ) )
		{
			const path file( itr->path() );
			m_vPaths.push_back( make_pair( file, file_size( file ) ) );
		}


}

FileGrabber::~FileGrabber() {}

const PATH_VECTOR& FileGrabber::getPaths() const
{
	return m_vPaths;
}

