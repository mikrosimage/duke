#ifndef COLOR_H_
#define COLOR_H_

#include <iostream>

class Color
{
public: Color( unsigned long Color = 0xFFFFFFFF );
	Color( unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xFF );

	unsigned char ToGrey() const;
	unsigned long ToARGB() const;

	unsigned long ToABGR() const;
	unsigned long ToRGBA() const;

	unsigned char getAlpha() const;
	unsigned char getRed() const;
	unsigned char getGreen() const;
	unsigned char getBlue() const;

	void SetFloats( float r, float g, float b, float a = 1.0f );
	void Set( unsigned char r, unsigned char g, unsigned char b, unsigned char a = 0xFF );
	void ToFloat( float Dest[] ) const;

	bool operator         ==( const Color& c ) const;
	bool operator         !=( const Color& c ) const;
	const Color& operator +=( const Color& c );
	const Color& operator -=( const Color& c );
	Color operator        +( const Color& c ) const;
	Color operator        -( const Color& c ) const;
	Color operator        *( float v ) const;
	const Color& operator *=( float v );
	Color operator        /( float v ) const;
	const Color& operator /=( float v );

	Color Add( const Color& c ) const;
	Color Modulate( const Color& c ) const;

	static const Color White;
	static const Color Black;
	static const Color Red;
	static const Color Green;
	static const Color Blue;

	friend std::istream & operator >>( std::istream& Stream, Color& Color );
	friend std::ostream & operator <<( std::ostream& Stream, const Color& Color );

private:
	void SetInt( int r, int g, int b, int a = 0xFF );

	unsigned long m_Color;
};

#endif // COLOR_H_
