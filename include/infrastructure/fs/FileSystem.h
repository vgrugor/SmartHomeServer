#ifndef FILE_SYSTEM_H
    #define FILE_SYSTEM_H

    #include <Arduino.h>
    #include <LittleFS.h>

    class FileSystem {
        public:
            FileSystem();
            bool writeFile(const String& path, const String& data);
            String readFile(const String& path);
            bool deleteFile(const String& path);
            bool fileExists(const String& path);
    };

#endif // FILE_SYSTEM_H
