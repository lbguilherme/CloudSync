#include <RemoteStorage/RemoteStorage.hpp>
#include <RemoteStorage/RemoteStorageService.hpp>
#include <Poco/Data/SQLite/Connector.h>
#include <iostream>
using namespace std;

void handle_exception();

int main() {
    set_terminate(handle_exception);
    // Poco::Data::SQLite::Connector::registerConnector();

    // RemoteStorage::createDatabase("dropbox-test.db");
    RemoteStorageService::loadServices();
    cout << endl << endl << RemoteStorageService::get("Dropbox")->name() << endl;
}
