#include "IResource.h"
#include <iostream>
#include <typeinfo>

IResource::IResource()
	{}

IResource::IResource( const std::string& name )
	: m_sName( name ) {}

IResource::~IResource()
{
	//	std::cout << "resource " << m_Name << " destroyed" << std::endl;
}
