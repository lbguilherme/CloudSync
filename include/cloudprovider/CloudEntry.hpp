#pragma once
#include <string>

class CloudEntry {
public:
    std::string name() {
        return _name;
    }

private:
    std::string _name;
};
