/*
 * GlObjectsDetails.h
 *
 *  Created on: Feb 15, 2013
 *      Author: Guillaume Chatelet
 */

#ifndef GLOBJECTSDETAILS_H_
#define GLOBJECTSDETAILS_H_

namespace duke {
namespace gl {

template<class GLOBJECT>
struct _Unbinder {
	void operator()(GLOBJECT *ptr) const {
		ptr->unbind();
	}
};

template<class GLOBJECT> using _PTR = std::unique_ptr<GLOBJECT, _Unbinder<GLOBJECT> >;

template<class GLOBJECT>
struct Binder: public _PTR<GLOBJECT> {
	Binder(const GLOBJECT* ptr) : _PTR<GLOBJECT>(const_cast<GLOBJECT*>(ptr)) {ptr->bind();}
	Binder(Binder &&other) = default;
	Binder() = default;
};

} /* namespace gl */
} /* namespace duke */

#endif /* GLOBJECTSDETAILS_H_ */
