/*
 * IDeclaration.h
 *
 *  Created on: 18 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef DECLARATION_H_
#define DECLARATION_H_

#include <boost/shared_ptr.hpp>

class IDeclaration
{
public:
	virtual ~IDeclaration() {}

protected: IDeclaration() {}
};

typedef ::boost::shared_ptr<IDeclaration> DeclarationPtr;

#endif /* DECLARATION_H_ */
