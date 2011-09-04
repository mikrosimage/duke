#include "foo.h"
#include <iostream>

#ifdef _WIN32
__declspec(dllexport)
#endif
void foo() {
    std::cout << "Hello from Foo." << std::endl;
}
