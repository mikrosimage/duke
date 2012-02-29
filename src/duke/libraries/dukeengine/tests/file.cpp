#include <dukeengine/memory/alloc/Allocators.h>
#include <dukeengine/memory/alloc/MemoryBlock.h>
#include <dukeengine/file/DmaFileIO.h>
#include <dukeengine/file/StreamedFileIO.h>
#include <boost/filesystem.hpp>

#include <iostream>
#include <fstream>

#define BOOST_TEST_MODULE FileTests
#include <boost/test/unit_test.hpp>

using namespace std;

const size_t filesize = 1 * 1024 * 1024; // 1MB
const char* filename = "temporary_file.bin";

const char* g_MemAlloc = (char*) malloc(filesize);

BOOST_AUTO_TEST_SUITE( FileSuite )

struct TempFile
{
    TempFile()
    {
        memset( (void*)g_MemAlloc, 0x7F, filesize );
        ofstream file( filename, ios_base::out|ios_base::trunc|ios_base::binary );
        file.write( g_MemAlloc, filesize );
        file.close();
    }

    ~TempFile()
    {
        ::boost::filesystem::remove( filename );
    }

};

void testReadFile( FileIO* const pFileIO, const char* filename )
{
    MemoryBlockPtr ptr( pFileIO->read( filename ) );
    const size_t size = ptr->size();
    char* pBuffer = ptr->getPtr<char>();

    BOOST_CHECK( pBuffer != NULL );
    BOOST_CHECK( size == filesize );
    BOOST_CHECK( memcmp( pBuffer, g_MemAlloc, size ) == 0 );
}

::mikrosimage::alloc::AlignedMallocAllocator alignedMallocAlloc;
::mikrosimage::alloc::MallocAllocator mallocAlloc;
::mikrosimage::alloc::NewAllocator newAlloc;
#if defined ( WINDOWS )
::mikrosimage::alloc::VirtualAllocator virtualAlloc;
#endif

BOOST_AUTO_TEST_CASE( FileTest1 )
{
    TempFile tempFile;

    {
        StreamedFileIO fileIo( &alignedMallocAlloc );
        testReadFile( &fileIo, filename );
    }
    {
        StreamedFileIO fileIo( &mallocAlloc );
        testReadFile( &fileIo, filename );
    }
    {
        StreamedFileIO fileIo( &newAlloc );
        testReadFile( &fileIo, filename );
    }
#if defined ( WINDOWS )
    {
        DmaFileIO fileIo( &alignedMallocAlloc );
        testReadFile( &fileIo, filename );
    }
    {
        StreamedFileIO fileIo( &virtualAlloc );
        testReadFile( &fileIo, filename );
    }
    {
        DmaFileIO fileIo( &virtualAlloc );
        testReadFile( &fileIo, filename );
    }
#endif
}

BOOST_AUTO_TEST_SUITE_END()
