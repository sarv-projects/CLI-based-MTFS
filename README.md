


# Multi-threaded File Transfer System

A robust and secure command-line interface (CLI) application for transferring files between a client and a server.  
Built with resilience in mind, it handles network interruptions gracefully and ensures data integrity through a custom protocol and strong encryption.

---

## 🚀 Key Features

- **CLI-Based Interface** – Run as either a server or a client with simple command-line arguments.  
- **Multi-threaded Server** – Spawns a new thread per client connection, supporting concurrent transfers.  
- **Chunked File Transfer** – Breaks files into smaller chunks to improve reliability.  
- **Automatic Resume** – Resumes file transfers from the exact point of interruption.  
- **AES-256 Encryption** – End-to-end encryption using OpenSSL for secure file transmission.  

---

## 🛠️ Technologies Used

- **Languages**: C++, Bash  
- **Libraries**: `pthread` (multi-threading), TCP sockets (network communication), OpenSSL (AES-256 encryption)  
- **Tools**: `g++` (compilation), Git (version control)  

---

## ⚙️ Installation & Setup

This project is designed for **Debian-based Linux systems (Ubuntu, etc.)**.  
The included `app.sh` script will auto-check and install required dependencies.

### 1. Clone the Repository
```bash
git clone https://github.com/sarv-projects/CLI-based-MTFS.git
cd your-repo-name
````

### 2. Make the Script Executable

```bash
chmod +x app.sh
```

### 3. Run the Script (First Time)

The script will:

* Check for `libssl-dev` (OpenSSL development libraries)
* Auto-install it if missing
* Compile the C++ sources into an executable (`file_transfer_app`)

---

## ▶️ Usage

The application is run via the `app.sh` script.

### Server Mode

Start the server (listens on **port 8080** by default):

```bash
./app.sh server
```

### Client Mode

Transfer a file to the server:

```bash
./app.sh client <server_ip_address> <filename_to_upload>
```

#### Example

```bash
./app.sh client 192.168.1.5 my_important_file.zip
```

If the transfer is interrupted, rerun the same command.
The system will **resume exactly where it left off**, without re-sending completed chunks.

---

## 📌 Notes

* Ensure both **server** and **client** machines have network connectivity.
* You can run server and client on the same machine for testing using `127.0.0.1` (localhost).
* AES-256 encryption requires OpenSSL; installation is handled by the script.

---

## 📄 License

MIT License – feel free to use and modify.

