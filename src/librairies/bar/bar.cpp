#include "bar.h"
#include <iostream>

#ifdef _WIN32
__declspec(dllexport)
#endif
void bar() {
    std::cout << "Hello from Bar." << std::endl;
}
