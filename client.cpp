#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <mutex>
#include <thread>
#include <openssl/aes.h> // Add this header
#include <openssl/rand.h> // Add this header
#include "common.h"
using namespace std;
void runClient(const std::string& host, const std::string& filename) {
    int clientSocket;
    struct sockaddr_in serverAddress;

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(8080);

    if (inet_pton(AF_INET, host.c_str(), &serverAddress.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    threadSafeLog("Connected to server.");

    string stateFilename = filename + ".state";
    long long resumePosition = 0;

    ifstream stateFile(stateFilename);
    if (stateFile.is_open()) {
        stateFile >> resumePosition;
        threadSafeLog("Found state file. Resuming from position: " + to_string(resumePosition) + " bytes.");
    }
    stateFile.close();

    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) {
        threadSafeLog("Error opening file: " + filename);
        close(clientSocket);
        return;
    }
    long long fileSize = file.tellg();
    file.seekg(0, ios::beg);

    FileHeader header;
    strncpy(header.filename, filename.c_str(), sizeof(header.filename));
    header.filename[sizeof(header.filename) - 1] = '\0';
    header.filesize = fileSize;
    header.resume_pos = resumePosition;
    send(clientSocket, &header, sizeof(header), 0);

    file.seekg(resumePosition);

    // --- Encryption Setup ---
    // You must use a 32-byte key for 256-bit AES
    // Ensure this key is identical to the one on the server side
    unsigned char key[] = "S2A5R0v6e2S0H25S2A5R0v6e2S0H25_"; 
    unsigned char encrypted_buffer[1024];
    AES_KEY enc_key;
    if (AES_set_encrypt_key(key, 256, &enc_key) < 0) {
        threadSafeLog("Error setting encryption key.");
        close(clientSocket);
        return;
    }

    // --- Transfer Loop with Encryption ---
    ofstream outputFile(stateFilename);
    char buffer[1024];
    long long totalBytesSent = resumePosition;

    while (file.read(buffer, 1024)) {
        int bytes_read = file.gcount();
        
        // Encrypt the buffer in 16-byte blocks
        for (int i = 0; i < bytes_read; i += 16) {
            AES_encrypt((unsigned char*)buffer + i, encrypted_buffer + i, &enc_key);
        }
        
        send(clientSocket, (char*)encrypted_buffer, bytes_read, 0);
        
        totalBytesSent += bytes_read;
        outputFile.seekp(0);
        outputFile << totalBytesSent; 
    }
    outputFile.close();

    file.close();
    close(clientSocket);

    threadSafeLog("File transfer complete. Total bytes sent: " + to_string(totalBytesSent) + ".");
}