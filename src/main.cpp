#include <cloudprovider/CloudProvider.hpp>
#include <Poco/Data/SQLite/Connector.h>

int main() {
    Poco::Data::SQLite::Connector::registerConnector();
    CloudProvider::createDatabase("dropbox-test.db");
}
