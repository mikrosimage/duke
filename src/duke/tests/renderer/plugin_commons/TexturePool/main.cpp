#include <plugin/TexturePool.h>
#include <iostream>
#include <boost/integer_traits.hpp>
#define BOOST_TEST_MODULE TexturePool
#include <boost/test/unit_test.hpp>

using namespace std;

class DummyResource : public IResource
{
int& m_Counter;

public: DummyResource( int& counter )
		: m_Counter( counter )
	{
		++m_Counter;
	}

	~DummyResource()
	{
		--m_Counter;
	}

	virtual const ::resource::Type getResourceType() const { return ::resource::UNDEFINED; }
	virtual std::size_t getSize() const { return sizeof(DummyResource); }

};

BOOST_AUTO_TEST_SUITE( TexturePoolSuite )

BOOST_AUTO_TEST_CASE( operatorLessOnKey )
{
	const PoolRequest RGBA8_NoMipMap( PXF_R8G8B8A8, false, 0, 0, 0 );
	const PoolRequest RGBA8_MipMap( PXF_R8G8B8A8, true, 0, 0, 0 );
	const PoolRequest BGRA8_NoMipMap( PXF_B8G8R8A8, false, 0, 0, 0 );
	PoolRequestLess less;

	BOOST_CHECK( !less( RGBA8_NoMipMap, RGBA8_NoMipMap ) );
	BOOST_CHECK( less( RGBA8_NoMipMap, RGBA8_MipMap ) );
	BOOST_CHECK( less( RGBA8_NoMipMap, BGRA8_NoMipMap ) );
}

BOOST_AUTO_TEST_CASE( testAcquireAndAutoRelease )
{
	int counter                         = 0;
	DummyResource* const pDummyResource = new DummyResource( counter );

	BOOST_CHECK( counter == 1 );
	{
		TexturePool pool;
		const PoolRequest request( PXF_R8G8B8A8, false, 0, 0, 0 );
		{
			TexturePool::ScopedTexturePtr pTexture( pool.get( request ) );
			BOOST_CHECK( !pTexture );
			BOOST_CHECK( counter == 1 );
		}
		{
			pool.put( request, ResourcePtr( pDummyResource ));
			TexturePool::ScopedTexturePtr pFirstGrab( pool.get( request ) );
			BOOST_CHECK( pFirstGrab );
			BOOST_CHECK( pFirstGrab->getTexture<DummyResource>() == pDummyResource );
			TexturePool::ScopedTexturePtr pSecondGrab( pool.get( request ) );
			BOOST_CHECK( !pSecondGrab );
			BOOST_CHECK( counter == 1 );
		}
		{
			// texture returned to the pool so we can retake it
			TexturePool::ScopedTexturePtr pGrabAgain( pool.get( request ) );
			BOOST_CHECK( pGrabAgain );
			BOOST_CHECK( pGrabAgain->getTexture<DummyResource>() == pDummyResource );
			BOOST_CHECK( counter == 1 );
		}
		BOOST_CHECK( counter == 1 );
	}
	// pool destruction should release all the resources
	BOOST_CHECK( counter == 0 );
}

BOOST_AUTO_TEST_CASE( testDistanceComputation )
{
	const TVector3I zero( 0, 0, 0 );
	const TVector3I one( 1, 1, 1 );

	BOOST_CHECK( TexturePool::computeDistance( zero, zero ) == 0 );
	BOOST_CHECK( TexturePool::computeDistance( one, one ) == 0 );
	BOOST_CHECK( TexturePool::computeDistance( one, zero ) == boost::integer_traits<int>::const_max );
	BOOST_CHECK( TexturePool::computeDistance( zero, one ) == 3 );
}

BOOST_AUTO_TEST_CASE( testBestSuited )
{
	const PoolRequest initialRequest( PXF_R8G8B8A8 , false, 200, 200 );
	const PoolRequest compatibleRequest( PXF_R8G8B8A8, false, 100, 100 );
	const PoolRequest imcompatibleRequest( PXF_R8G8B8A8, false, 300, 300 );
	const PoolRequest imcompatibleRequestWithMipMap( PXF_R8G8B8A8, true, 200, 200 );
	const PoolRequest imcompatibleRequestWithAnotherFormat( PXF_B8G8R8A8, false, 200, 200 );
	TexturePool pool;
	int counter              = 0;
	DummyResource* pResource = new DummyResource( counter );

	pool.put( initialRequest, ResourcePtr( pResource ) );
	{
		BOOST_CHECK( pool.get( initialRequest ) );
	}
	// resource should have returned to pool so we can get it again
	{
		TexturePool::ScopedTexturePtr pTexture( pool.get( initialRequest ) );
		BOOST_CHECK( pTexture );
		BOOST_CHECK( pTexture->getTexture<DummyResource>() == pResource );
	}
	{
		TexturePool::ScopedTexturePtr pTexture( pool.get( compatibleRequest ) );
		BOOST_CHECK( pTexture );
		BOOST_CHECK( pTexture->getTexture<DummyResource>() == pResource );
	}
	{
		TexturePool::ScopedTexturePtr pTexture( pool.get( imcompatibleRequest ) );
		BOOST_CHECK( !pTexture );
	}
	{
		TexturePool::ScopedTexturePtr pTexture( pool.get( imcompatibleRequestWithMipMap ) );
		BOOST_CHECK( !pTexture );
	}
	{
		TexturePool::ScopedTexturePtr pTexture( pool.get( imcompatibleRequestWithAnotherFormat ) );
		BOOST_CHECK( !pTexture );
	}
}
BOOST_AUTO_TEST_SUITE_END()
