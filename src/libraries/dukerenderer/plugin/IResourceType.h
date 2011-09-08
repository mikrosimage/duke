/*
 * IResourceType.h
 *
 *  Created on: 3 sept. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef IRESOURCETYPE_H_
#define IRESOURCETYPE_H_


namespace resource {

enum Type {
	UNDEFINED, MESH, IMAGE, SHADER, TEXTURE, PROTOBUF
};

inline const char* TypeToString(const Type type){
	switch (type) {
		case UNDEFINED:
			return "UNDEFINED";
		case MESH:
			return "MESH";
		case IMAGE:
			return "IMAGE";
		case SHADER:
			return "SHADER";
		case TEXTURE:
			return "TEXTURE";
		case PROTOBUF:
			return "PROTOBUF";
	}
	return "UNDEFINED";
}

}  // namespace resource

#endif /* IRESOURCETYPE_H_ */
