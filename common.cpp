#include "common.h"
#include<iostream>
// Define the global log mutex
std::mutex logMutex; 

// Define the threadSafeLog function
void threadSafeLog(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::cout << message << std::endl;
}