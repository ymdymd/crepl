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

static
int& get_symbol_val(const std::string& name, void* _this) {
	return (*static_cast<std::map<std::string, int>*>(_this))[name];
}

int main(int argc, char** argv)
{
	std::map<std::string, int> symbol;
	std::string s;
	do {
		if (s.empty()) {	//dump all symbol & value	
			for (auto itr = symbol.begin(); itr != symbol.end(); ++itr) {
				std::cout << itr->first          
					<< " = " << itr->second << "\n";
			}
		}
		else {	// evalute expresion
			std::cout << expr::eval(s, get_symbol_val,&symbol) << std::endl;
		}
		std::cout << ">";
	} while (std::getline(std::cin, s));
}

