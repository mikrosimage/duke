#ifndef VECTOR4_H_
#define VECTOR4_H_

#include <limits>
#include <iostream>

template<class T>
class Vector4
{
public: Vector4( T X = 0, T Y = 0, T Z = 0, T W = 1 );

	void Set( T X, T Y, T Z, T W );

	T Length() const;

	T LengthSq() const;

	void Normalize();

	Vector4<T> operator +() const;
	Vector4<T> operator -() const;

	Vector4<T> operator +( const Vector4<T>& v ) const;
	Vector4<T> operator -( const Vector4<T>& v ) const;

	const Vector4<T>& operator +=( const Vector4<T>& v );
	const Vector4<T>& operator -=( const Vector4<T>& v );

	Vector4<T> operator *( T t ) const;
	Vector4<T> operator /( T t ) const;

	const Vector4<T>& operator *=( T t );
	const Vector4<T>& operator /=( T t );

	bool operator ==( const Vector4<T>& v ) const;
	bool operator !=( const Vector4<T>& v ) const;

	operator T *();

	T x;
	T y;
	T z;
	T w;
};

template<class T>
Vector4<T> operator *( const Vector4<T>& v, T t );
template<class T>
Vector4<T> operator /( const Vector4<T>& v, T t );
template<class T>
Vector4<T> operator *( T t, const Vector4<T>& v );
template<class T>
T VectorDot( const Vector4<T>& v1, const Vector4<T>& v2 );
template<class T>
Vector4<T> VectorCross( const Vector4<T>& v1, const Vector4<T>& v2 );
template<class T>
std::istream & operator >>( std::istream& Stream, Vector4<T>& vector );
template<class T>
std::ostream & operator <<( std::ostream& Stream, const Vector4<T>& vector );

typedef Vector4<int> TVector4I;
typedef Vector4<float> TVector4F;

#include "Vector4.inl"

#endif // VECTOR4_H_
