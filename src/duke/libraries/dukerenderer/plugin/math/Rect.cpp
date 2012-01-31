#include "Rect.h"

Rect::Rect( const TVector2I& start, const TVector2I& size )
	: Origin( start ),
	End( start + size ) {}

Rect::Rect( int left, int top, int width, int height )
	: Origin( left, top ),
	End( left + width, top + height ) {}

void Rect::set( int left, int top, int width, int height )
{
	Origin.Set( left, top );
	End.Set( left + width, top + height );
}

int Rect::left() const
{
	return Origin.x;
}

int Rect::right() const
{
	return End.x;
}

int Rect::top() const
{
	return Origin.y;
}

int Rect::bottom() const
{
	return End.y;
}

int Rect::width() const
{
	return End.x - Origin.x;
}

int Rect::height() const
{
	return End.y - Origin.y;
}

TVector2I Rect::size() const
{
	return End - Origin;
}

TIntersection Rect::intersects( const TVector2I& point ) const
{
	if( ( point.x >= Origin.x ) && ( point.y >= Origin.y ) && ( point.x <= End.x ) && ( point.y <= End.y ) )
		return INT_IN;
	else
		return INT_OUT;
}

TIntersection Rect::intersects( const Rect& rect ) const
{
	TVector2I Start( std::max( Origin.x, rect.Origin.x ), std::max( Origin.y, rect.Origin.y ) );
	TVector2I localEnd( std::min( End.x, rect.End.x ), std::min( End.y, rect.End.y ) );
	Rect Overlap( Start, localEnd - Start );

	if( ( Start.x > End.x ) || ( Start.y > End.y ) )
		return INT_OUT;
	else if( ( Overlap == *this ) || ( Overlap == rect ) )
		return INT_IN;
	else
		return INT_INTERSECTS;
}

bool Rect::operator ==( const Rect& rect ) const
{
	return ( Origin == rect.Origin ) && ( End == rect.End );
}

bool Rect::operator !=( const Rect& rect ) const
{
	return !( *this == rect );
}

std::istream & operator >>( std::istream& stream, Rect& rect )
{
	return stream >> rect.Origin >> rect.End;
}

std::ostream & operator <<( std::ostream& stream, const Rect& rect )
{
	return stream << rect.Origin << " " << rect.End;
}

