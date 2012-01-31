#ifndef RECTANGLE_H_
#define RECTANGLE_H_

#include "Vector2.h"
#include <iostream>

enum TIntersection
{
	INT_IN, INT_OUT, INT_INTERSECTS
};

class Rect
{
public: Rect( const TVector2I& start = TVector2I( 0, 0 ), const TVector2I& size = TVector2I( 0, 0 ) );
	Rect( int left, int top, int Width, int height );

	void set( int left, int top, int Width, int height );

	int       left() const;
	int       right() const;
	int       top() const;
	int       bottom() const;
	int       width() const;
	int       height() const;
	TVector2I size() const;

	TIntersection intersects( const TVector2I& point ) const;
	TIntersection intersects( const Rect& rect ) const;

	bool operator ==( const Rect& rect ) const;
	bool operator !=( const Rect& rect ) const;

	TVector2I Origin;
	TVector2I End;
};

std::istream & operator >>( std::ostream& stream, Rect& rect );
std::ostream & operator <<( std::ostream& stream, const Rect& rect );

#endif // _RECTANGLE_H_
