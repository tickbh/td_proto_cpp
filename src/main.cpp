#include "Values.h"
#include "Config.h"
#include <iostream>
#include <string>
using namespace td_proto;

int test(Values value){
	std::cout << "xxxxxxxxxxx" << std::endl;
	return 0;
}

int main(int argc, char *argv[]) {
	float xx = 1.0;
	auto value = Values(xx);
	{
		Values strValue = Values(new std::string("fkkk"));
		Values strValue1 = Values(std::move(strValue));
		//test(std::move(strValue1));
		//test(strValue1);


		//std::cout << "aaaa" << std::endl;
		//Values v = std::move(strValue1);
		Values v = strValue1;
	}
	std::cout << "....." << std::endl;
}