#include "Configuration.h"

#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char** argv) {
    try {
        Configuration configuration(argc, argv);
        return configuration.m_iReturnValue;
    } catch (exception& e) {
        cerr << "Unexpected error : " << e.what() << endl;
    } catch (...) {
        cerr << "Unexpected error." << endl;
    }
    return EXIT_FAILURE;
}
