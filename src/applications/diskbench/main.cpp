#include "Configuration.h"
#include <iostream>

using namespace std;

int main( int argc, char** argv )
{
	try
	{
		Configuration configuration( argc, argv );
		return configuration.m_iReturnValue;
	}
	catch( exception& e )
	{
		cerr << "Unexpected error : " << e.what() << endl;
	}
	catch( ... )
	{
		cerr << "Unexpected error." << endl;
	}
	return 1;
}

