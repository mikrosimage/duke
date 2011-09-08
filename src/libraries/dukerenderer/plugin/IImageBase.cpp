/*
 * IImageBase.cpp
 *
 *  Created on: 2 juin 2010
 *      Author: Guillaume Chatelet
 */

#include "IImageBase.h"
#include "utils/PixelUtils.h"

IImageBase::IImageBase( const ImageDescription& imageDescription )
	: m_Description( imageDescription ),
	m_Pixels( m_Description.width * m_Description.height * (m_Description.depth==0?1:m_Description.depth) * GetBytesPerPixel( m_Description.format ) ) {}

IImageBase::~IImageBase() {}

const ImageDescription& IImageBase::getImageDescription() const
{
	return m_Description;
}

