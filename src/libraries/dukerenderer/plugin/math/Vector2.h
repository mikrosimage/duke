#ifndef VECTOR2_H_
#define VECTOR2_H_

#include <iostream>

template<class T>
class Vector2
{
public: Vector2( T X = 0, T Y = 0 );

	void Set( T X, T Y );

	T Length() const;

	T LengthSq() const;

	void Normalize();

	Vector2<T> operator +() const;
	Vector2<T> operator -() const;

	Vector2<T> operator +( const Vector2<T>& v ) const;
	Vector2<T> operator -( const Vector2<T>& v ) const;

	const Vector2<T>& operator +=( const Vector2<T>& v );
	const Vector2<T>& operator -=( const Vector2<T>& v );

	Vector2<T> operator *( T t ) const;
	Vector2<T> operator /( T t ) const;

	const Vector2<T>& operator *=( T t );
	const Vector2<T>& operator /=( T t );

	bool operator ==( const Vector2<T>& v ) const;
	bool operator !=( const Vector2<T>& v ) const;

	operator T *();

	T x;
	T y;
};

template<class T>
Vector2<T> operator *( const Vector2<T>& v, T t );
template<class T>
Vector2<T> operator /( const Vector2<T>& v, T t );
template<class T>
Vector2<T> operator *( T t, const Vector2<T>& v );
template<class T>
T VectorDot( const Vector2<T>& v1, const Vector2<T>& v2 );
template<class T>
Vector2<T> VectorCross( const Vector2<T>& v1, const Vector2<T>& v2 );
template<class T>
std::istream & operator >>( std::istream& stream, Vector2<T>& vector );
template<class T>
std::ostream & operator <<( std::ostream& stream, const Vector2<T>& vector );

typedef Vector2<int> TVector2I;
typedef Vector2<float> TVector2F;

#include "Vector2.inl"

#endif // VECTOR2_H_
