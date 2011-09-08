/*
 * Mesh.h
 *
 *  Created on: 11 mai 2010
 *      Author: Guillaume Chatelet
 */

#ifndef MESH_H_
#define MESH_H_

#include <communication.pb.h>
#include <boost/shared_ptr.hpp>

class IFactory;
class IRenderer;
class IMeshBase;

class Mesh
{
public: Mesh( IFactory&, const ::protocol::duke::Mesh& );
	virtual ~Mesh();

	void render( IRenderer& ) const;

private:
	::boost::shared_ptr<IMeshBase> m_pMeshBase;
};

#endif /* MESH_H_ */
