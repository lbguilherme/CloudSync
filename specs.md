## Main Goals

- Be faster than usual synchronization programs
- Support multiple services and multiple accounts per service
- Be lazy on downloading everything, but fast for the user
- Be cross-platform
- Be very simple to use

## Code Organization

1. RemoteStorage: A `RemoteStorageService` is a external service that provides remote storage, such as *Dropbox*. A `RemoteStorage` is one connected account from one such service. It will provide means to listen for changes and apply file operation remotely. The interface should be unified for all services by using virtual methods. The `RemoteStorageService` class will be aware of the service limitations/features. The implementation for those things is independent from the rest and is service specific. If possible, use the official public API of each one. Each service will be defined as a plug-in loaded on startup (a `.so` file).

2. LocalStore: One `LocalStore` connects to a `RemoteStorage` and locally store a representation of the remote file system. This local representation should be kept in sync with a worker thread (or something similar). A SQLite database will be used to store metadata about all files and directories, some other form of storage *to-be-decided* will be used to store the actual data. Any request coming to the `LocalStore` will be answered with local data if possible, otherwise it will fallback to asking the `RemoteStorage` (slow). Keeping metadata in sync is of highest priority.

3. VirtualFileSystem: A `VirtualFileSystem` looks like a real file system for the final user. He can see it on any desktop application such as nautilus, just like a normal directory or hard drive. Every action the user takes, such as opening a directory or renaming a file will cause a function on our code to be called. On Linux this would be implemented with *libFUSE*, on Windows with *Dokany*, and on Mac with some variation of *macFUSE*.

4. UI: UI to managing all this stuff using Qt for portability. To be decided later. Ideally modules 1,2,3 should be on a executable that listens for HTTP requests on a local socket, and the UI should be a fully separated application that makes requests. This design is commonly known as *daemon*. Note: lowest priority. For now UI will be on terminal, with ugly whatever stubs.
