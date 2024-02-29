#include <iostream>

#define EXPORT __declspec(dllexport)

extern "C" EXPORT void dostuff(double x, double y, double __stdcall p( double, double ) )
{
	std::cout << "hello from c++" << std::endl;
	std::cout << x + y << ", " << p(x, y) << std::endl;
}

int main()
{
	return 0;
}