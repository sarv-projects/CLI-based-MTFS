#!/bin/bash

# --- Check for Dependencies ---
echo "Checking for required dependencies..."

# Check if OpenSSL development libraries are installed
if ! dpkg -s libssl-dev &>/dev/null; then
    echo "OpenSSL development libraries (libssl-dev) not found. Attempting to install..."
    sudo apt-get update
    sudo apt-get install -y libssl-dev
    if [ $? -ne 0 ]; then
        echo "Error: Failed to install libssl-dev. Please install it manually."
        exit 1
    fi
    echo "OpenSSL development libraries installed successfully."
fi

# Define the name of the executable file
APP_NAME="file_transfer_app"

# --- Part 1: Auto-Compile ---
# This checks if the executable exists. If not, it builds the project using g++.
if [ ! -f "$APP_NAME" ]; then
    echo "Executable not found. Compiling project now..."
    g++ main.cpp client.cpp server.cpp common.cpp -o "$APP_NAME" -pthread -std=c++17 -lssl -lcrypto
 
    if [ $? -ne 0 ]; then
        echo "Error: Compilation Failed."
        exit 1
    fi
    echo "Compilation successful! Executable file created."
fi

# --- Part 2: Handle Commands ---
if [ $# -lt 1 ]; then
    echo "Usage: ./app.sh [server | client <host> <filename>]"
    exit 1
fi

MODE=$1
shift

if [ "$MODE" == "server" ]; then
    echo "Starting server..."
    ./"$APP_NAME" server
elif [ "$MODE" == "client" ]; then
    if [ $# -lt 2 ]; then
        echo "Error: Client mode requires a host and filename."
        echo "Usage: ./app.sh client <host> <filename>"
        exit 1
    fi
    HOST=$1
    FILENAME=$2
    echo "Starting client to transfer '$FILENAME' to '$HOST'..."
    ./"$APP_NAME" client "$HOST" "$FILENAME"
else
    echo "Invalid mode specified. Use 'server' or 'client'."
    exit 1
fi
