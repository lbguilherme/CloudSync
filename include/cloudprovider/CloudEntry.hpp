#pragma once
#include <string>

class CloudEntry {
public:
    inline const std::string& name() const {
        return _name;
    }

private:
    std::string _name;
};
