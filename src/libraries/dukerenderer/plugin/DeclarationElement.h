/*
 * Buffer.h
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */
#ifndef DECLARATIONELEMENT_H_
#define DECLARATIONELEMENT_H_

enum TElementUsage
{
	ELT_USAGE_POSITION, ELT_USAGE_NORMAL, ELT_USAGE_DIFFUSE, ELT_USAGE_TEXCOORD0, ELT_USAGE_TEXCOORD1, ELT_USAGE_TEXCOORD2, ELT_USAGE_TEXCOORD3
};

enum TElementType
{
	ELT_TYPE_FLOAT1, ELT_TYPE_FLOAT2, ELT_TYPE_FLOAT3, ELT_TYPE_FLOAT4, ELT_TYPE_COLOR
};

struct TDeclarationElement
{
	unsigned int Stream;
	TElementUsage Usage;
	TElementType DataType;
};

#endif // DECLARATIONELEMENT_H_
