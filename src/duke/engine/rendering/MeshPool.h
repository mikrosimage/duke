/*
 * MeshPool.h
 *
 *  Created on: Mar 2, 2013
 *      Author: Guillaume Chatelet
 */

#pragma once

#include <duke/gl/Mesh.h>

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
