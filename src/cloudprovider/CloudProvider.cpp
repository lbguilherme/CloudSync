#include <cloudprovider/CloudProvider.hpp>
#include <Poco/Data/Session.h>

using namespace Poco::Data;

void CloudProvider::createDatabase(const std::string& databasePath) {
    Session session("SQLite");
    session.open(databasePath);
}

CloudProvider* CloudProvider::fromDatabase(const std::string& databasePath) {
    Session session("SQLite");
    session.open(databasePath);

    return nullptr;
}
