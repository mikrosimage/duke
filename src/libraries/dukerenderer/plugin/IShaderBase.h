#ifndef ISHADERBASE_H_
#define ISHADERBASE_H_

#include "Enums.h"
#include "IResource.h"
#include "math/Matrix4.h"
#include <Cg/cg.h>
#include <string>

class IShaderBase : public IResource
{
public: IShaderBase( CGprogram program, TShaderType type );
	virtual ~IShaderBase();

	CGprogram   getProgram() const;
	TShaderType getType() const;

	void setParameter( const std::string& name, const float* value, const int size, const bool logIfInexistent = true ) const;
	const std::vector<std::string>& getParameterNames() const;
	CGparameter  getParameter( const std::string& name ) const;

	virtual const ::resource::Type getResourceType() const { return ::resource::SHADER; }
	virtual std::size_t getSize() const { return sizeof(IShaderBase); }
protected:
	virtual void setParameter( const CGparameter parameter, const float* value, const int size ) const = 0;
	void appendParameter( const CGparameter );
	CGprogram m_Program;
	const TShaderType m_Type;
	std::vector<std::string> m_ParameterNames;
};

#include <boost/shared_ptr.hpp>
typedef ::boost::shared_ptr<IShaderBase> ShaderBasePtr;

#endif /* ISHADERBASE_H_ */
