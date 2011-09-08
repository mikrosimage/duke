/*
 * IResource.h
 *
 *  Created on: 10 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef IRESOURCE_H_
#define IRESOURCE_H_

#include "IResourceType.h"

#include <boost/noncopyable.hpp>
#include <string>

class ResourceManager;

class IResource : private ::boost::noncopyable
{
private:
	friend class ResourceManager;
public:
	IResource();
	virtual ~IResource();

	const std::string& getName() const { return m_sName; }

	virtual const resource::Type getResourceType() const = 0;
	virtual std::size_t getSize() const = 0;

protected:
	IResource( const std::string& name );

private:
	std::string m_sName;
};

#include <boost/shared_ptr.hpp>
typedef ::boost::shared_ptr<IResource> ResourcePtr;

#endif /* IRESOURCE_H_ */
