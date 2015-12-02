#pragma once
#include <string>

class RemoteStorage {
public:
    static void createDatabase(const std::string& databasePath);
    static RemoteStorage* fromDatabase(const std::string& databasePath);

private:
    RemoteStorage();
};
