#include <RemoteStorage-Dropbox/DropboxService.hpp>

using namespace std;

extern "C" RemoteStorageService* serviceFactory() {
    return new DropboxService();
}

string DropboxService::name() {
    return "Dropbox";
}
