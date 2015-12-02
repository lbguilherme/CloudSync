#pragma once
#include <RemoteStorage/RemoteStorageService.hpp>

class DropboxService : public RemoteStorageService {
public:
    virtual std::string name() override;
};
