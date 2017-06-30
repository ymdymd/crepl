#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <list>
#include <regex>
#include <memory>
#include <map>
#include "expr.h"


//=============================================================================
//main

int main(int argc, char** argv)
{
	std::string s;
	do {
		if (!s.empty()) {
			std::cout << expr::eval(s) << std::endl;
		}
		std::cout << ">";
	} while (std::getline(std::cin, s));
}

