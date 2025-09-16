cmake_minimum_required(VERSION 3.10)
project(file_transfer_app CXX)

# Find necessary libraries
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Add your C++ executable
add_executable(file_transfer_app main.cpp client.cpp server.cpp)

# Link the libraries to the executable
target_link_libraries(file_transfer_app PRIVATE Threads::Threads OpenSSL::SSL OpenSSL::Crypto)