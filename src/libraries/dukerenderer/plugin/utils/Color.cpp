#include "Color.h"

const Color Color::White( 255, 255, 255 );
const Color Color::Black( 0, 0, 0 );
const Color Color::Red( 255, 0, 0 );
const Color Color::Green( 0, 255, 0 );
const Color Color::Blue( 0, 0, 255 );

Color::Color( unsigned long Color )
	: m_Color( Color )
{}

Color::Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
	Set( r, g, b, a );
}

void Color::SetFloats( float r, float g, float b, float a )
{
	int R = static_cast<int>( r * 255.0f );
	int G = static_cast<int>( g * 255.0f );
	int B = static_cast<int>( b * 255.0f );
	int A = static_cast<int>( a * 255.0f );

	SetInt( R, G, B, A );
}

void Color::Set( unsigned char r, unsigned char g, unsigned char b, unsigned char a )
{
	m_Color = ( a << 24 ) | ( r << 16 ) | ( g << 8 ) | ( b << 0 );
}

void Color::SetInt( int r, int g, int b, int a )
{
	unsigned char R = ( r >= 0 ) ? ( r <= 255 ? r : 255 ) : 0;
	unsigned char G = ( g >= 0 ) ? ( g <= 255 ? g : 255 ) : 0;
	unsigned char B = ( b >= 0 ) ? ( b <= 255 ? b : 255 ) : 0;
	unsigned char A = ( a >= 0 ) ? ( a <= 255 ? a : 255 ) : 0;

	Set( R, G, B, A );
}

bool Color::operator ==( const Color& c ) const
{
	return m_Color == c.m_Color;
}

bool Color::operator!=( const Color& c ) const
{
	return !( *this == c );
}

const Color& Color::operator +=( const Color& c )
{
	int R = getRed()   + c.getRed();
	int G = getGreen() + c.getGreen();
	int B = getBlue()  + c.getBlue();
	int A = getAlpha() + c.getAlpha();

	SetInt( R, G, B, A );

	return *this;
}

const Color& Color::operator -=( const Color& c )
{
	int R = getRed()   - c.getRed();
	int G = getGreen() - c.getGreen();
	int B = getBlue()  - c.getBlue();
	int A = getAlpha() - c.getAlpha();

	SetInt( R, G, B, A );

	return *this;
}

Color Color::operator +( const Color& c ) const
{
	int R = getRed()   + c.getRed();
	int G = getGreen() + c.getGreen();
	int B = getBlue()  + c.getBlue();
	int A = getAlpha() + c.getAlpha();

	Color Ret;

	Ret.SetInt( R, G, B, A );

	return Ret;
}

Color Color::operator -( const Color& c ) const
{
	int R = getRed()   - c.getRed();
	int G = getGreen() - c.getGreen();
	int B = getBlue()  - c.getBlue();
	int A = getAlpha() - c.getAlpha();

	Color Ret;

	Ret.SetInt( R, G, B, A );

	return Ret;
}

Color Color::operator *( float v ) const
{
	int R = static_cast<int>( getRed()   * v );
	int G = static_cast<int>( getGreen() * v );
	int B = static_cast<int>( getBlue()  * v );
	int A = static_cast<int>( getAlpha() * v );

	Color Ret;

	Ret.SetInt( R, G, B, A );

	return Ret;
}

const Color& Color::operator *=( float v )
{
	int R = static_cast<int>( getRed()   * v );
	int G = static_cast<int>( getGreen() * v );
	int B = static_cast<int>( getBlue()  * v );
	int A = static_cast<int>( getAlpha() * v );

	SetInt( R, G, B, A );

	return *this;
}

Color Color::operator /( float v ) const
{
	return *this * ( 1.0f / v );
}

const Color& Color::operator /=( float v )
{
	return *this *= ( 1.0f / v );
}

Color Color::Add( const Color& c ) const
{
	return *this + c;
}

Color Color::Modulate( const Color& c ) const
{
	unsigned char R = static_cast<unsigned char>( getRed()   * c.getRed()   / 255 );
	unsigned char G = static_cast<unsigned char>( getGreen() * c.getGreen() / 255 );
	unsigned char B = static_cast<unsigned char>( getBlue()  * c.getBlue()  / 255 );
	unsigned char A = static_cast<unsigned char>( getAlpha() * c.getAlpha() / 255 );

	return Color( R, G, B, A );
}

unsigned char Color::getAlpha() const
{
	return static_cast<unsigned char>( ( m_Color & 0xFF000000 ) >> 24 );
}

unsigned char Color::getRed() const
{
	return static_cast<unsigned char>( ( m_Color & 0x00FF0000 ) >> 16 );
}

unsigned char Color::getGreen() const
{
	return static_cast<unsigned char>( ( m_Color & 0x0000FF00 ) >> 8 );
}

unsigned char Color::getBlue() const
{
	return static_cast<unsigned char>( ( m_Color & 0x000000FF ) >> 0 );
}

void Color::ToFloat( float Dest[] ) const
{
	Dest[0] = getRed()   / 255.0f;
	Dest[1] = getGreen() / 255.0f;
	Dest[2] = getBlue()  / 255.0f;
	Dest[3] = getAlpha() / 255.0f;
}

unsigned char Color::ToGrey() const
{
	return static_cast<unsigned char>( getRed() * 0.30 + getGreen() * 0.59 + getBlue() * 0.11 );
}

unsigned long Color::ToARGB() const
{
	return ( getAlpha() << 24 ) | ( getRed() << 16 ) | ( getGreen() << 8 ) | ( getBlue() << 0 );
}

unsigned long Color::ToABGR() const
{
	return ( getAlpha() << 24 ) | ( getBlue() << 16 ) | ( getGreen() << 8 ) | ( getRed() << 0 );
}

unsigned long Color::ToRGBA() const
{
	return ( getRed() << 24 ) | ( getGreen() << 16 ) | ( getBlue() << 8 ) | ( getAlpha() << 0 );
}

std::istream & operator >>( std::istream& Stream, Color& Color )
{
	int R, G, B, A;

	Stream >> R >> G >> B >> A;
	Color.SetInt( R, G, B, A );

	return Stream;
}

std::ostream & operator <<( std::ostream& Stream, const Color& Color )
{
	return Stream << static_cast<int>( Color.getRed() )   << " "
	       << static_cast<int>( Color.getGreen() ) << " "
	       << static_cast<int>( Color.getBlue() )  << " "
	       << static_cast<int>( Color.getAlpha() );
}

