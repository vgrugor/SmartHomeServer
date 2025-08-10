#include "infrastructure/fs/FileSystem.h"

FileSystem::FileSystem() {
    LittleFS.begin();
}

bool FileSystem::writeFile(const String& path, const String& data) {
    File file = LittleFS.open(path, "w");

    if (!file) {
        return false;
    }

    file.print(data);
    file.close();

    return true;
}

String FileSystem::readFile(const String& path) {
    File file = LittleFS.open(path, "r");

    if (!file) {
        return "";
    }

    String data = file.readString();
    file.close();

    return data;
}

bool FileSystem::deleteFile(const String& path) {
    return LittleFS.remove(path);
}

bool FileSystem::fileExists(const String& path) {
    return LittleFS.exists(path);
}
