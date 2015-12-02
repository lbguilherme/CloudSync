#include <RemoteStorage/RemoteStorageService.hpp>
#include <Poco/DirectoryIterator.h>
#include <Poco/SharedLibrary.h>
#include <regex>
#include <iostream>

using namespace Poco;
using namespace std;

map<string, RemoteStorageService*> RemoteStorageService::services;

void RemoteStorageService::loadServices() {
    string path = ".";
    DirectoryIterator end;
    for (DirectoryIterator it(path); it != end; ++it) {
        if (it->isDirectory() || !it->canExecute())
            continue;

        if (regex_match(it->path(), regex("libRemoteStorage-.*\\.so")))
            loadService(it->path());
    }
}

void RemoteStorageService::loadService(const string& path) {
    clog << "Loading service " << path << " ... ";

    using factoryFunc = RemoteStorageService*();

    SharedLibrary lib;
    lib.load(path);
    factoryFunc* factory = reinterpret_cast<factoryFunc*>(lib.getSymbol("serviceFactory"));

    RemoteStorageService* service = factory();
    string name = service->name();
    if (services.count(name)) {
        throw "Attempt to load '" + name + "' twice";
    }

    services[name] = service;
    clog << name << endl;
}

RemoteStorageService* RemoteStorageService::get(const std::string name) {
    return services.at(name);
}
