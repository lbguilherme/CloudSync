#pragma once
#include <string>

class CloudProvider {
public:
    static void createDatabase(const std::string& databasePath);
    static CloudProvider* fromDatabase(const std::string& databasePath);

private:
    CloudProvider();
};
