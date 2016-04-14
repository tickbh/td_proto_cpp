#include "Values.h"
#include <iostream>
#include <string>
using namespace td_proto;

int main(int argc, char *argv[]) {
	float xx = 1.0;
	auto value = Values(xx);
	{
		auto strValue = Values(new std::string("fkkk"));
	}
	std::cout << "....." << std::endl;
}