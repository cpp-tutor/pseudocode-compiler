#include <iostream>
#include <vector>
#include <variant>
#include <any>
#include <unordered_map>
#include <set>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <stack>
#include "SymbolTypes.hpp"
#include "Tree.hpp"
#include "Expression.hpp"
#include "Symbol.hpp"
#include "Parser.hpp"
#include <FlexLexer.h>
#include "Lexer.hpp"
#include <fstream>
#include <sstream>
#include <span>
#include <string_view>

using namespace std::literals::string_view_literals;

const std::vector<std::pair<std::string_view,std::string_view>> Options{
#ifdef _MSC_VER
    { "/?", "/help" },
    { "/v", "/version" },
    { "/n", "/nodejs" },
#else
    { "-h", "--help" },
    { "-v", "--version" },
    { "-n", "--nodejs" },
#endif
};

const auto support_nodejs_code = 
        R"(const readline_sync = require('readline-sync');
function readline() {
    return readline_sync.question('');
}
function print(s) {
    console.log(s);
}
)";

int main(const int argc, const char *argv[]) {
    std::span<const char *> args(argv + 1, argv + argc);
    bool support_nodejs{ false }, cgi_program{ getenv("GATEWAY_INTERFACE") != nullptr };
    std::istream *infile{ nullptr };
    std::ostream *outfile{ nullptr };
    std::ostringstream error_stream;
    int returncode{ 1 };

    if (auto query_string = getenv("QUERY_STRING"); cgi_program && query_string) {
        args = args.subspan(0, 0);
        std::cout << "Content-type: text/javascript\n\n";
        auto key_str = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_="sv;
        const char *p = query_string;
        std::string read_input;
        while (*p) {
            unsigned char enc1, enc2, enc3, enc4;
            enc1 = key_str.find(*p++);
            enc2 = key_str.find(*p++);
            enc3 = key_str.find(*p++);
            enc4 = key_str.find(*p++);
            read_input.push_back((enc1 << 2) | (enc2 >> 4));
            if (enc3 < 64) {
                read_input.push_back((enc2 << 4) | (enc3 >> 2));
            }
            if (enc4 < 64) {
                read_input.push_back((enc3 << 6) | enc4);
            }
        }
        read_input.push_back('\n');
        infile = new std::istringstream{ read_input };
    }
    while (!args.empty() && (args.front()[0] == Options.front().first[0])) {
        auto opt{ -1 };
        for (const auto& o : Options) {
            if (args.front() == o.first || args.front() == o.second) {
                opt = &o - &Options[0];
                break;
            }
        }
        switch (opt) {
            case 0:
                std::cerr << "Syntax: " << argv[0] << " [";
                std::cerr << Options[0].first << '|' << Options[1].first << '|' << Options[2].first;
                std::cerr << "] [input [output]]\n\n";
                std::cerr << "   " << Options[0].first << '\t' << Options[0].second;
                std::cerr << "\n\t\tOutputs this usage info.\n";
                std::cerr << "   " << Options[1].first << '\t' << Options[1].second;
                std::cerr << "\n\t\tOutputs version info.\n";
                std::cerr << "   " << Options[2].first << '\t' << Options[2].second;
                std::cerr << "\n\t\tAdds Node.JS support library code to JavaScript output.\n\n";
                std::cerr << "If [output] is not present, the output filename is constructed by removing the\n";
                std::cerr << "suffix (if any) from the input filename and adding \'.js\'.\n\n";
                std::cerr << "If [input] is not present, then input is read from the keyboard and output is\n";
                std::cerr << "written to the console (after ^Z/^D).\n";
                return returncode;
            case 1:
                std::cerr << "Version 0.30 [release] 2022/03/03\n";
                return returncode;
            case 2:
                support_nodejs = true;
                break;
            default:
                std::cerr << "Error: unsupported option: " << args.front() << '\n';
                return returncode;
        }
        args = args.last(args.size() - 1);
    }

    if (!args.empty()) {
        infile = new std::ifstream{ args.front() };
        if (!*infile) {
            std::cerr << "Error: could not open file for reading: " << args.front() << '\n';
            return returncode;
        }
    }
    if (infile && *infile) {
        std::cin.rdbuf(infile->rdbuf());
        if (args.size() == 1) {
            std::string outfilename{ args.front() };
            outfile = new std::ofstream{ outfilename.replace(outfilename.find('.'), std::string::npos, ".js") };
        }
        else if (args.size() == 2) {
            outfile = new std::ofstream{ args.back() };
        }
        if (outfile && *outfile) {
            std::cout.rdbuf(outfile->rdbuf());
        }
        else if (!cgi_program) {
            std::cerr << "Error: could not open file for writing: " << args.back() << '\n';
            return returncode;
        }
    }
    yy::Lexer scanner{};
    Symbol table{};
    Decls start{ &table };
    yy::Parser parser{ &scanner, &table, &start, &error_stream };

    try {
        if (!parser.parse()) {
            if (support_nodejs) {
                std::cout << support_nodejs_code;
            }
            start.emit();
            returncode = 0;
        }
    }
    catch (std::exception& e) {
        error_stream << "Fatal compiler inconsistency: " << e.what() << '\n';
    }
    catch (...) {
        error_stream << "Fatal compiler inconsistency: unknown\n";
    }
    if (error_stream.str().size()) {
        if (cgi_program) {
            std::string msg = error_stream.str();
            auto pos = std::min(msg.find('\n'), msg.find('\r'));
            if (pos != std::string::npos) {
                msg = msg.erase(pos);
            }
            std::cout << "print(\"" << msg << "\");\n";
        }
        else {
            std::cerr << error_stream.str();
        }
    }
    return returncode;
}
