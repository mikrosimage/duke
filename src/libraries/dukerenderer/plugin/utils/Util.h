#ifndef UTIL_H_
#define UTIL_H_

inline unsigned int NearestPowerOfTwo( unsigned int value )
{
	unsigned int temp       = value;
	unsigned int powerOfTwo = 0;

	while( temp > 1 )
	{
		temp >>= 1;
		++powerOfTwo;
	}

	unsigned int retval = 1 << powerOfTwo;

	return retval == value ? retval : retval << 1;
}

#endif // UTIL_H_
