/*
 * MeshPool.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef MESHPOOL_H_
#define MESHPOOL_H_

#include <duke/gl/Mesh.hpp>

namespace duke {

struct MeshPool {
	SharedMesh getSquare() const {
		if (!m_pSquare)
			m_pSquare = duke::createSquare();
		return m_pSquare;
	}
	SharedMesh getLine() const {
		if (!m_pLine)
			m_pLine = duke::createLine();
		return m_pLine;
	}
private:
	mutable SharedMesh m_pSquare;
	mutable SharedMesh m_pLine;
};

}  // namespace duke

#endif /* MESHPOOL_H_ */
