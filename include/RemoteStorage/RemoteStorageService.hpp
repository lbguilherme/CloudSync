#pragma once
#include <string>
#include <map>

class RemoteStorageService {
    static std::map<std::string, RemoteStorageService*> services;

public:
    static void loadServices();
    static void loadService(const std::string& path);
    static RemoteStorageService* get(const std::string name);

    virtual std::string name() = 0;
};
