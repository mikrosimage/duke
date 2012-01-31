#ifndef VECTOR3_H_
#define VECTOR3_H_

#include <limits>
#include <iostream>

template<class T>
class Vector3
{
public: Vector3( T X = 0, T Y = 0, T Z = 0 );

	void Set( T X, T Y, T Z );

	T Length() const;

	T LengthSq() const;

	void Normalize();

	Vector3<T> operator +() const;
	Vector3<T> operator -() const;

	Vector3<T> operator +( const Vector3<T>& v ) const;
	Vector3<T> operator -( const Vector3<T>& v ) const;

	const Vector3<T>& operator +=( const Vector3<T>& v );
	const Vector3<T>& operator -=( const Vector3<T>& v );

	const Vector3<T>& operator *=( T t );
	const Vector3<T>& operator /=( T t );

	bool operator ==( const Vector3<T>& v ) const;
	bool operator !=( const Vector3<T>& v ) const;

	operator T *();

	T x;
	T y;
	T z;
};

template<class T>
Vector3<T> operator *( const Vector3<T>& v, T t );
template<class T>
Vector3<T> operator /( const Vector3<T>& v, T t );
template<class T>
Vector3<T> operator *( T t, const Vector3<T>& v );
template<class T>
T VectorDot( const Vector3<T>& v1, const Vector3<T>& v2 );
template<class T>
Vector3<T> VectorCross( const Vector3<T>& v1, const Vector3<T>& v2 );
template<class T>
std::istream & operator >>( std::istream& stream, Vector3<T>& Vector );
template<class T>
std::ostream & operator <<( std::ostream& stream, const Vector3<T>& Vector );

typedef Vector3<int> TVector3I;
typedef Vector3<float> TVector3F;

#include "Vector3.inl"

#endif // VECTOR3_H_
