#include <execinfo.h>
#include <cxxabi.h>
#include <iostream>
#include <iomanip>
#include <regex>
using namespace std;

static void replace(string& s, const string& toReplace, const string& replaceWith) {
    size_t pos;
    while ((pos = s.find(toReplace)) != string::npos)
        s.replace(pos, toReplace.length(), replaceWith);
}

static void replace(string& s, const regex& toReplace, const string& replaceWith) {
    s = regex_replace(s, toReplace, replaceWith);
}

static string demangle(string symbol) {
    int status;
    char* output = abi::__cxa_demangle(symbol.c_str(), 0, 0, &status);
    if (status == 0) {
        symbol = output;
        free(output);
        replace(symbol, "std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >", "std::string");
        replace(symbol, regex("std::map<(.+?), (.+?), std::less<\\1 >, std::allocator<std::pair<\\1 const, \\2> > >"), "std::map<$1, $2>");
        replace(symbol, regex("std::vector<(.+?), std::allocator<\\1 > >"), "std::vector<$1>");
    }
    return symbol;
}

void handle_exception() {
    cerr << "\n\n########## Exception ##########\n\n";

    try {
        if (current_exception()) {
            rethrow_exception(current_exception());
        }
    } catch (const std::exception& e) {
        string name = demangle(typeid(e).name());
        cout << "Caught exception "
             << "\033[1;33m" << name << "\033[0m";
        cerr << ": "
             << "\033[1;37m" << e.what() << "\033[0m" << endl;
    } catch (const string& msg) {
        cout << "Caught exception: "
             << "\033[1;37m" << msg << "\033[0m" << endl;
    } catch (const char* msg) {
        cout << "Caught exception: "
             << "\033[1;37m" << msg << "\033[0m" << endl;
    } catch (...) {
        type_info* ex = __cxxabiv1::__cxa_current_exception_type();
        if (ex) {
            string name = demangle(ex->name());
            cout << "Caught exception "
                 << "\033[1;33m" << name << "\033[0m" << endl;
        }
    }
    cerr << endl;

    void* trace_elems[20];
    int trace_elem_count(backtrace(trace_elems, 20));
    char** stack(backtrace_symbols(trace_elems, trace_elem_count));
    for (int i = 1; i < trace_elem_count; ++i) {
        cmatch match;
        if (regex_match(stack[i], match, regex("^([^(]+)\\((.*?)\\+?(0x[a-f0-9]+)?\\) \\[0x([a-f0-9]+)\\]$"))) {
            string path = match[1];
            string symbol = match[2];
            string delta = match[3];
            string addr = match[4];

            cerr << "\033[0;31m"
                 << "0x" << setw(12) << setfill('0') << addr << setw(0) << ": "
                 << "\033[0m";

            cerr << "\033[0;34m" << path << " "
                 << "\033[0m";

            if (symbol.length() > 0) {
                symbol = demangle(symbol);
                cerr << "\033[1;33m" << symbol << "\033[0m";
                cerr << "+" << delta;
            }

            cerr << endl;
        } else {
            cerr << stack[i] << endl;
        }
    }
    cerr << endl;
}
