/*
 * MapRemoveIf.h
 *
 *  Created on: Feb 16, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef MAPREMOVEIF_H_
#define MAPREMOVEIF_H_

#include <algorithm>

template<typename Map, typename F>
void map_erase_if(Map& m, F pred) {
	for (auto i = std::begin(m); (i = std::find_if(i, std::end(m), pred)) != std::end(m);)
		m.erase(i++);
}

#endif /* MAPREMOVEIF_H_ */
