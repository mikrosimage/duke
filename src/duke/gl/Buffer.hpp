#ifndef BUFFER_HPP_
#define BUFFER_HPP_

#include <duke/NonCopyable.h>

#include <GL/gl.h>

template<class POLICY>
struct Buffer: public NonCopyable {
public:
	typedef POLICY Policy;
	Buffer(GLuint targetType) :
			id(Policy::allocate()), targetType(Policy::checkTargetType(targetType)) {
	}
	~Buffer() {
		Policy::deallocate(id);
	}
	const GLuint id;
	const GLuint targetType;
};

#include <memory>

template<class BUFFER>
struct ScopeBinder {
	ScopeBinder(const std::shared_ptr<BUFFER> pBuffer) :
			m_TargetType(pBuffer->targetType) {
		BUFFER::Policy::bind(m_TargetType, pBuffer->id);
	}
	ScopeBinder(const BUFFER &buffer) :
			m_TargetType(buffer.targetType) {
		BUFFER::Policy::bind(m_TargetType, buffer.id);
	}
	~ScopeBinder() {
		BUFFER::Policy::unbind(m_TargetType);
	}

private:
	const GLuint m_TargetType;
};

#endif /* BUFFER_HPP_ */
