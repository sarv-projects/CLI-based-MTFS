#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <thread>
#include <vector>
#include "common.h"
using namespace std;
void runServer();//imported from the server.cpp file
void runClient(const std::string& host,const std::string& filename);//imported from the client.cpp file
using namespace std;
int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./app <server|client> [host] [filename]" << std::endl;
        return 1;
    }

    std::string mode = argv[1];

    if (mode == "server") {
        runServer();
    } else if (mode == "client") {
        if (argc < 4) {
            std::cerr << "Client mode requires a host and filename argument." << std::endl;
            return 1;
        }
        std::string host = argv[2]; // Get the server's IP address from arguments
        std::string filename = argv[3]; // Get the filename
        runClient(host, filename);
    } else {
        std::cerr << "Invalid mode. Please use 'server' or 'client'." << std::endl;
        return 1;
    }
    return 0;
}