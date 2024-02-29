#include <iostream>

int main( int argc, char* argv[] )
{
#ifdef CLIENT
    std::cout << "Client" << std::endl;
#elif SERVER
    std::cout << "Server" << std::endl;
#endif
}