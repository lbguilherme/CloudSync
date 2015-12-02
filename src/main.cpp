#include <RemoteStorage/RemoteStorage.hpp>
#include <RemoteStorage/RemoteStorageService.hpp>
#include <Poco/Data/SQLite/Connector.h>
#include <iostream>
using namespace std;

int main() {
    // Poco::Data::SQLite::Connector::registerConnector();

    // RemoteStorage::createDatabase("dropbox-test.db");
    RemoteStorageService::loadServices();
    cout << endl << endl << RemoteStorageService::get("Dropbox")->name() << endl;
}
