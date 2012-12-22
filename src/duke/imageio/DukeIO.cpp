/*
 * DukeIO.cpp
 *
 *  Created on: Dec 15, 2012
 *      Author: Guillaume Chatelet
 */

#include "DukeIO.h"
#include <cassert>
#include <stdexcept>

using namespace std;



bool IODescriptors::registerDescriptor(IIODescriptor* pDescriptor) {
	assert(pDescriptor);
	m_Descriptors.push_back(unique_ptr<IIODescriptor>(pDescriptor));
	for (const string& extension : pDescriptor->getSupportedExtensions())
		m_ExtensionToDescriptors[extension].push_back(pDescriptor);
	return true;
}

const vector<IIODescriptor*>& IODescriptors::findDescriptor(const char* extension) const {
	const auto pFound = m_ExtensionToDescriptors.find(extension);
	if (pFound != m_ExtensionToDescriptors.end())
		return pFound->second;
	static const vector<IIODescriptor*> EmptyVector;
	return EmptyVector;
}

IODescriptors& IODescriptors::instance() {
	static IODescriptors descriptors;
	return descriptors;
}
