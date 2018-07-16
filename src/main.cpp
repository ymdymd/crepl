#include "expr.h"
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#define USE_EDITLINE

#ifdef USE_EDITLINE
#include <editline/history.h>
#include <editline/readline.h>
#endif

//-----------------------------------------------------------------------------
#include <cstdarg>
static std::string format_str(const char *fmt, ...) {
    static char buf[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return std::string(buf);
}

//-----------------------------------------------------------------------------
static int &get_symbol_val(const std::string &name, void *_this) {
    return (*static_cast<std::map<std::string, int> *>(_this))[name];
}

//=============================================================================
// main
int main(int argc, char **argv) {
    std::map<std::string, int> symbol;
#ifdef USE_EDITLINE
    char *line;
    using_history();
    read_history(NULL); // [ToDo]historyファイルが無いときの動作の検証
    while (1) {
        line = readline(">> ");
        if (*line == 0) { // dump all symbol & value
            for (auto itr = symbol.begin(); itr != symbol.end(); ++itr) {
                std::cout << itr->first << " = " << itr->second << "\n";
            }
        } else { // evalute expresion
            add_history(line);
            try {
                int val = expr::eval(line, get_symbol_val, &symbol);
                std::cout << format_str("(0x%08x) %d\n", val, val);
            } catch (const std::runtime_error &e) {
                std::cout << e.what() << std::endl;
            }
        }

        free(line);
    }
    write_history(NULL);
#else

    std::string s;
    do {
        if (s.empty()) { // dump all symbol & value
            for (auto itr = symbol.begin(); itr != symbol.end(); ++itr) {
                std::cout << itr->first << " = " << itr->second << "\n";
            }
        } else { // evalute expresion
            try {
                int val = expr::eval(s, get_symbol_val, &symbol);
                std::cout << format_str("(0x%08x) %d\n", val, val);
            } catch (const std::runtime_error &e) {
                std::cout << e.what() << std::endl;
            }
        }
        std::cout << ">";
    } while (std::getline(std::cin, s));
#endif
}
