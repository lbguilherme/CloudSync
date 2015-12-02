#include <execinfo.h>
#include <cxxabi.h>
#include <iostream>
#include <iomanip>
#include <regex>
#include <dlfcn.h>
#include <unistd.h>
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

static string exec(const string& cmd) {
    shared_ptr<FILE> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
        return "ERROR";
    char buffer[128];
    string result = "";
    while (!feof(pipe.get())) {
        if (fgets(buffer, 128, pipe.get()) != NULL)
            result += buffer;
    }
    return result;
}

string pwd() {
    char buf[999];
    getcwd(buf, 999);
    return buf;
}

static void cleanpath(string& path) {
    if (path.size() > 0 && *path.rbegin() == '\n')
        path.pop_back();

    if (path.find(pwd()) != string::npos) {
        replace(path, pwd() + "/", "");
        path = "\033[1;37m" + path + "\033[0m";
    }

    int n = 10;
    while (n--) {
        path = regex_replace(path, regex("/(.*?)/../"), "/");
    }
}

void handle_exception() {
    cerr << "\n\n########## Exception ##########\n\n";

    try {
        if (current_exception()) {
            rethrow_exception(current_exception());
        }
    } catch (const exception& e) {
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
            string source;

            void* addrptr;
            stringstream ss(addr);
            ss >> hex >> addrptr;
            unsigned long symbolAddr = (unsigned long)addrptr;

            Dl_info info;
            dladdr(addrptr, &info);
            if (info.dli_saddr) {
                symbolAddr = (unsigned long)addrptr - (unsigned long)info.dli_fbase;
            }

            stringstream command;
            command << "addr2line 0x" << hex << symbolAddr << " -e " << path;
            source = exec(command.str());
            if (source.size() > 0 && source[0] == '?')
                source = "";
            cleanpath(source);

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
            cerr << "\t" << source << endl;
        } else {
            cerr << stack[i] << endl;
        }
    }
    cerr << endl;
}

__attribute__((constructor)) static void setup_exception_handler() {
    set_terminate(handle_exception);
}
