/*
 * TexturePool.cpp
 *
 *  Created on: 31 mai 2010
 *      Author: Guillaume Chatelet
 */

#include "TexturePool.h"
#include "IResource.h"
#include "utils/PixelUtils.h"

#include <boost/foreach.hpp>
#include <boost/integer_traits.hpp>

#include <cassert>

TexturePool::TexturePool() {}

TexturePool::~TexturePool()
{
	if( !m_Map.empty() )
	{
		std::cout << "TexturePool : freeing resources\n";

		for( MAP::const_iterator pEntry = m_Map.begin(); pEntry != m_Map.end(); ++pEntry )
		{
			const RecyclingTexturePtrList& ptrList( pEntry->second );
			for( RecyclingTexturePtrList::const_iterator pTextureHolder = ptrList.begin(); pTextureHolder != ptrList.end(); ++pTextureHolder )
			{
			    const PoolRequest& request( ( *pTextureHolder )->m_Request );
				std::cout << " - " << request.dimension.x << "x" << request.dimension.y << "\t" << FormatToString( request.format ) << std::endl;
			}
		}
	}
}

TexturePool::TextureHolder::TextureHolder( const PoolRequest& request, const ResourcePtr& pResource )
	: m_Request( request ),
	m_pResource( pResource ) {}

ScopedTexture::ScopedTexture()
	: m_pPool( NULL ) {}

ScopedTexture::ScopedTexture( TexturePool* const pPool, TexturePool::TextureHolderPtr pRecylingTexture )
	: m_pPool( pPool ),
	m_pRecylingTexture( pRecylingTexture )
{
	assert( m_pPool );
}

ScopedTexture::~ScopedTexture()
{
	if( m_pRecylingTexture != NULL )
		m_pPool->put( m_pRecylingTexture->m_Request, m_pRecylingTexture->m_pResource );
}

void TexturePool::put( const PoolRequest& request, const ResourcePtr& pResource )
{
	put( TextureHolderPtr( new TextureHolder( request, pResource ) ) );
}

TexturePool::ScopedTexturePtr TexturePool::putAndGet( const PoolRequest& request, const ResourcePtr& pResource )
{
	return ScopedTexturePtr( new ScopedTexture( this, TextureHolderPtr( new TextureHolder( request, pResource ) ) ) );
}

void TexturePool::put( TextureHolderPtr pRecylingTexture )
{
	assert( pRecylingTexture != NULL );
	m_Map[pRecylingTexture->m_Request].push_front( pRecylingTexture );
}

int TexturePool::computeDistance( const TVector3I& requestedDimension, const TVector3I& testDimension )
{
	if( requestedDimension.x > testDimension.x || requestedDimension.y > testDimension.y || requestedDimension.z > testDimension.z )
		return boost::integer_traits<int>::const_max;
	return ( requestedDimension - testDimension ).LengthSq();
}

TexturePool::ScopedTexturePtr TexturePool::get( const PoolRequest& request )
{
	const MAP::iterator mapItr = m_Map.find( request );

	if( mapItr == m_Map.end() || mapItr->second.empty() )
		return ScopedTexturePtr();
	RecyclingTexturePtrList& list                  = mapItr->second;
	RecyclingTexturePtrList::iterator bestMatchItr = list.end();
	int bestMatchDistance                          = boost::integer_traits<int>::const_max;
	for( RecyclingTexturePtrList::iterator itr = list.begin(); itr != list.end(); ++itr )
	{
		const int distance = computeDistance( request.dimension, ( *itr )->m_Request.dimension );
		if( distance == 0 )
		{
			bestMatchItr = itr;
			break;
		}
		if( distance < bestMatchDistance )
		{
			bestMatchDistance = distance;
			bestMatchItr      = itr;
		}
	}
	if( bestMatchItr == list.end() )
		return ScopedTexturePtr();
	TextureHolderPtr ptr( *bestMatchItr );
	list.erase( bestMatchItr );
	return ScopedTexturePtr( new ScopedTexture( this, ptr ) );
}

