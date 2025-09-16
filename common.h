#ifndef COMMON_H
#define COMMON_H

#include <mutex>
#include <string>
#include <iostream>

// This struct will act as our file header
struct FileHeader {
    char filename[256];
    long long filesize;
    long long resume_pos;
};

// Declare the global log mutex using 'extern'
// 'extern' tells the compiler that the definition is in another file
extern std::mutex logMutex; 

// Declare the threadSafeLog function
void threadSafeLog(const std::string& message);

#endif