#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <fstream>
#include <vector>
#include <mutex>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include "common.h"
using namespace std;
void handleClient(int clientSocket) {
    threadSafeLog("Thread started to handle client connection.");
    
    // Receive file header
    FileHeader header;
    int bytes_received_header = recv(clientSocket, &header, sizeof(header), 0);
    if (bytes_received_header <= 0) {
        threadSafeLog("Client disconnected or failed to send file header.");
        close(clientSocket);
        return;
    }

    // Use the resume_position from the header
    threadSafeLog("Receiving file: " + string(header.filename) + " (" + to_string(header.filesize) + " bytes) from position " + to_string(header.resume_pos) + ".");
    
    // Open file for writing and seek to the correct position
    ofstream file(header.filename, ios::binary | ios::app);
    if (!file.is_open()) {
        threadSafeLog("Error opening file for writing.");
        close(clientSocket);
        return;
    }

    // Encryption setup
    // You must use a 32-byte key for 256-bit AES
    unsigned char key[] = "S2A5R0v6e2S0H25S2A5R0v6e2S0H25_"; // 32 characters long
    unsigned char decrypted_buffer[1024];
    AES_KEY dec_key;
    if (AES_set_decrypt_key(key, 256, &dec_key) < 0) {
        threadSafeLog("Error setting decryption key.");
        close(clientSocket);
        return;
    }

    // Receive file data in a loop
    char buffer[1024] = {0};
    long long totalBytesReceived = header.resume_pos;
    long long bytesToReceive = header.filesize - header.resume_pos;

    while (bytesToReceive > 0) {
        int bytes_read = recv(clientSocket, buffer, min(1024LL, bytesToReceive), 0);
        if (bytes_read <= 0) {
            break;
        }
        
        // Decrypt the buffer
        for (int i = 0; i < bytes_read; i += 16) {
            AES_decrypt((unsigned char*)buffer + i, decrypted_buffer + i, &dec_key);
        }

        // Write the decrypted buffer to the file
        file.write((char*)decrypted_buffer, bytes_read);
        totalBytesReceived += bytes_read;
        bytesToReceive -= bytes_read;
    }

    if (bytesToReceive > 0) {
        threadSafeLog("Receive failed. Transfer interrupted.");
    }
    
    file.close();
    close(clientSocket);
    threadSafeLog("File transfer complete. Total bytes received: " + to_string(totalBytesReceived));
}

void runServer() {
    int server_fd, new_client_socket;
    struct sockaddr_in server_addr;
    int addrlen = sizeof(server_addr);
    
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    threadSafeLog("Server listening on port 8080...");
    
    while (true) {
        if ((new_client_socket = accept(server_fd, (struct sockaddr *)&server_addr, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            continue;
        }
        
        threadSafeLog("Client connected. Spawning new thread.");
        
        thread clientThread(handleClient, new_client_socket);
        clientThread.detach();
    }
    
    close(server_fd);
}
