#include <RemoteStorage/RemoteStorage.hpp>
#include <Poco/Data/Session.h>

using namespace Poco::Data;

void RemoteStorage::createDatabase(const std::string& databasePath) {
    Session session("SQLite");
    session.open(databasePath);
}

RemoteStorage* RemoteStorage::fromDatabase(const std::string& databasePath) {
    Session session("SQLite", databasePath);

    return nullptr;
}
