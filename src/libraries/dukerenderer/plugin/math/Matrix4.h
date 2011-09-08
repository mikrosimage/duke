#ifndef MATRIX4_H_
#define MATRIX4_H_

#include "Vector3.h"
#include "Vector4.h"
#include <cmath>

class Matrix4
{
public: Matrix4( float m11 = 1.0f, float m12 = 0.0f, float m13 = 0.0f, float m14 = 0.0f,
	             float m21 = 0.0f, float m22 = 1.0f, float m23 = 0.0f, float m24 = 0.0f,
	             float m31 = 0.0f, float m32 = 0.0f, float m33 = 1.0f, float m34 = 0.0f,
	             float m41 = 0.0f, float m42 = 0.0f, float m43 = 0.0f, float m44 = 1.0f );

	void Identity();

	float Determinant() const;

	Matrix4 Transpose() const;

	Matrix4 Inverse() const;

	void SetTranslation( float x, float y, float z );

	void SetScaling( float x, float y, float z );

	void SetRotationX( float Angle );
	void SetRotationY( float Angle );
	void SetRotationZ( float Angle );

	void SetRotationX( float Angle, const TVector3F& Center );
	void SetRotationY( float Angle, const TVector3F& Center );
	void SetRotationZ( float Angle, const TVector3F& Center );

	TVector3F GetTranslation() const;

	TVector3F Transform( const TVector3F& v, float w = 1.0f ) const;
	TVector4F Transform( const TVector4F& v ) const;

	void OrthoOffCenter( float Left, float Top, float Right, float Bottom );

	void PerspectiveFOV( float Fov, float Ratio, float Near, float Far );

	void LookAt( const TVector3F& From, const TVector3F& To, const TVector3F& Up = TVector3F( 0, 1, 0 ) );

	Matrix4 operator +() const;
	Matrix4 operator -() const;

	Matrix4 operator +( const Matrix4& m ) const;
	Matrix4 operator -( const Matrix4& m ) const;

	const Matrix4& operator +=( const Matrix4& m );
	const Matrix4& operator -=( const Matrix4& m );

	Matrix4 operator        *( const Matrix4& m ) const;
	const Matrix4& operator *=( const Matrix4& m );

	const Matrix4& operator *=( float t );
	const Matrix4& operator /=( float t );

	bool operator ==( const Matrix4& m ) const;
	bool operator !=( const Matrix4& m ) const;

	float& operator       ()( std::size_t i, std::size_t j );
	const float& operator ()( std::size_t i, std::size_t j ) const;

	operator float *();
	operator const float *() const;

	float a11, a12, a13, a14;
	float a21, a22, a23, a24;
	float a31, a32, a33, a34;
	float a41, a42, a43, a44;
};

Matrix4 operator        *( const Matrix4& m, float t );
Matrix4 operator        *( float t, const Matrix4& m );
Matrix4 operator        /( const Matrix4& m, float t );
std::istream & operator >>( std::istream& Stream, Matrix4& Mat );
std::ostream & operator <<( std::ostream& Stream, const Matrix4& Mat );

#include "Matrix4.inl"

#endif // MATRIX4_H_
