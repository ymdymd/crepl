#include "expr.h"
#include "macro.h"
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
#include <utility>
static std::string format_str(const char *fmt, ...) {
    static char buf[2048];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    return std::string(buf);
}

//-----------------------------------------------------------------------------
static void version() {
    // clang-format off
    std::cout <<
"crepl (C-style Read Evalute Print Line) ver.0.0\n"
"(:q to quit, :? to help)\n"
;
    // clang-format on
}

//-----------------------------------------------------------------------------
static void help() {
    version();
    // clang-format off
    std::cout <<
"- Evalute equation\n"
"> 1 + 2 + 3\n"
"(0x00000006) 6\n"
"- Assign variable\n"
"> a = 1 + 2\n"
"(0x00000003) 3\n"
"- Show variable\n"
"> a\n"
"(0x00000003) 3\n"
"- Exit program\n"
"> :q\n"
"- print all variable\n"
"> :p\n"
"";
    // clang-format on
}

//-----------------------------------------------------------------------------
static void print(std::map<std::string, int> &symbols) {
    for (auto &itr : symbols) {
        std::cout << itr.first << " = " << itr.second << "\n";
    }
}

static void eval(const std::string line, std::map<std::string, int> &symbols) {
    auto get_symbol_ref = [&] (const std::string &symbol) -> int&{ 
        return symbols[symbol];
    };
  
    try {
        int val = expr::eval(line, get_symbol_ref);
        std::cout << format_str("(0x%08x) %d\n", val, val);
    } catch (const std::runtime_error &e) {
        std::cout << e.what() << std::endl;
    }
}

//=============================================================================
// main
int main(int argc, char **argv) {
    UNUSED(argc);
    UNUSED(argv);
    version();
    std::map<std::string, int> symbols;

#ifdef USE_EDITLINE
    using_history();
    // read_history(".history"); // [ToDo]historyファイルが無いときの動作の検証
    while (1) {
        char *buf = readline("> ");
        std::string line(buf);
        free(buf);
        if (line.empty()) {
            continue;
        }
        add_history(line.c_str());
        if (line == ":q") {
            break;
        }
        if (line == ":?") {
            help();
        }
        if (line == ":p") {
            print(symbols);
        } else { // evalute expresion
            eval(line, symbols);
        }
    }
// write_history(".history");
#else

    std::string s;
    do {
        if (s.empty()) { // dump all symbol & value
            for (auto itr = symbols.begin(); itr != symbols.end(); ++itr) {
                std::cout << itr->first << " = " << itr->second << "\n";
            }
        } else { // evalute expresion
            try {
                int val = expr::eval(s, get_symbol_ref);
                std::cout << format_str("(0x%08x) %d\n", val, val);
            } catch (const std::runtime_error &e) {
                std::cout << e.what() << std::endl;
            }
        }
        std::cout << ">";
    } while (std::getline(std::cin, s));
#endif
}
