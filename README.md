# 🚀 WebServ - Your Lightweight HTTP Server

<div align="center">
  <img src="https://github.com/Serghini04/WebServ/assets/intro.png" alt="WebServ Banner" width="800">

  ### *Power and simplicity in one elegant package*
  
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
  [![C++](https://img.shields.io/badge/C++-98-blue.svg)](https://isocpp.org/)
  [![Build Status](https://img.shields.io/badge/build-passing-green.svg)]()
</div>

## ✨ Welcome to WebServ

**WebServ** is not just another HTTP server — it’s a journey into the heart of web protocols, meticulously handcrafted in C++98. Designed from scratch, this lightweight yet powerful server delivers everything from static content to dynamic applications with efficiency and elegance.

---

## 🔍 Project Overview

WebServ implements the HTTP/1.1 protocol, mimicking the behavior of industry giants like **Nginx** and **Apache**, but with a focus on **simplicity**, **education**, and **clean architecture**.

Built with a modular, **SOLID principles**-driven design, WebServ offers:
- Multi-server support
- User authentication systems
- File uploads and downloads
- Dynamic content generation through **CGI (PHP + Python)** 
- Full HTTP status code management

It respects RFC standards and offers a minimal, robust, and extendable architecture — all without threading, using event-driven I/O with `select()`.

<div align="center">
  <img src="https://github.com/Serghini04/WebServ/assets/webserv-architecture.png" alt="WebServ Architecture" width="600">
</div>

---

## 📋 Table of Contents
- [✨ Features](#-features)
- [🧩 Architecture](#-architecture)
- [🛠️ Technologies](#️-technologies)
- [⚙️ Installation](#️-installation)
- [📝 Configuration](#-configuration)
- [🚦 Usage](#-usage)
- [💪 Challenges & Solutions](#-challenges--solutions)
- [👨‍💻 The Dream Team](#-the-dream-team)
- [🔮 Future Enhancements](#-future-enhancements)
- [📄 License](#-license)

---

## ✨ Features

### 🌐 Core Capabilities
- **🔄 Multi-server hosting**: Listen on multiple ports, serve different roots
- **📊 Efficient non-blocking I/O**: Lightning-fast responses
- **🔒 Session management**: Secure authentication flows
- **📁 File operations**: Robust upload and download system
- **⚡ Dynamic content**: **CGI support** for **PHP** and **Python** scripts
- **🛡️ Access control**: Restrict allowed HTTP methods
- **📑 Directory listing**: Beautiful, configurable auto-indexing

### 🚦 HTTP Protocol Support
- **GET**: 📥 Fetch resources
- **POST**: 📤 Submit forms or upload files
- **DELETE**: 🗑️ Remove resources
- **Status Codes**: ✅ Full HTTP status code management (2xx, 3xx, 4xx, 5xx)
- **URL Redirection**: 🔀 Simple redirects with HTTP status codes like 301, 302

### 📦 Content Processing
- **🧩 MIME type handling**
- **📏 Content-Length** validation
- **🔄 Chunked transfer encoding**
- **📂 Multipart/form-data** support
- **🗂️ URL encoding and decoding**

---

## 🧩 Architecture

WebServ follows a **modular architecture** adhering strictly to **SOLID principles**:

- **Single Responsibility**: Each class has one purpose (e.g., RequestParser, ResponseBuilder, ServerManager)
- **Open/Closed**: Extend behavior without modifying existing code
- **Liskov Substitution**: Interfaces are replaceable
- **Interface Segregation**: Only depend on what you use
- **Dependency Inversion**: Abstract high-level and low-level modules

**Core Components**:
- **Event-Driven Server Core**: Using `select()` for multiplexed I/O
- **Request Parser**: Handles fragmented HTTP messages
- **Response Generator**: Produces status lines, headers, and bodies
- **Configuration Manager**: Loads and validates server settings
- **CGI Handler**: Executes **Python** and **PHP** scripts
- **Session Manager**: Maintains login/auth state
- **Static File Manager**: Secure file serving and uploads

<div align="center">
  <img src="https://github.com/Serghini04/WebServ/assets/webserv-flow.png" alt="WebServ Data Flow" width="700">
</div>

---

## 🛠️ Technologies

- **C++98**: Full server logic
- **Socket Programming**: No external dependencies
- **Event-driven I/O**: `select()` based
- **POSIX APIs**: System interaction
- **Python / PHP**: For CGI scripts
- **HTTP/1.1 RFCs**: Protocol compliance

---

## ⚙️ Installation

```bash
# Clone the project
git clone https://github.com/Serghini04/WebServ.git

# Enter project directory
cd WebServ

# Build the server
make

# Launch the server
./webserv
```

---

## 📝 Configuration

The server is configured via a custom `.conf` file (`Configfile.conf`).  
Example:

```conf
server {
    server_name 1337;
    host 0.0.0.0;
    port 8080;
    root /Users/serghini/Desktop/WebServ/www;

    location / {
        allowed_methods GET;
        cgi .php /usr/bin/php;
        cgi .py /usr/bin/python3;
        auto_index off;
        index index.html;
    }

    location /upload {
        allowed_methods GET POST;
        upload_store /Users/serghini/uploads;
        auto_index on;
    }

    location /mehdi {
        return 302 https://profile.intra.42.fr/users/mehdi;
    }
}
```

### ⚡ Configuration Options
- **🏷️ server_name**: Human-friendly server name
- **🌐 host**: IP address to bind
- **🔌 port**: Listening port
- **📁 root**: Web root
- **🗺️ location blocks**: Route-specific rules
  - `allowed_methods`
  - `cgi` handlers
  - `auto_index` control
  - `upload_store` directories
- **📦 Redirection handling**

<div align="center">
  <img src="https://github.com/Serghini04/WebServ/assets/webserv-config.png" alt="WebServ Config" width="600">
</div>

---

## 🚦 Usage

Start the server with:

```bash
./webserv path/to/your/config.conf
```

Default ports: **8080** and **4444**

Browse to:
- `/` — Landing page
- `/register.html` — Registration form
- `/upload` — Upload files
- `/profile` — User profiles

---

## 💪 Challenges & Solutions

### 🔥 Handling Event-driven I/O Without Threads
**Solution**: Used `select()` for scalable multiplexed connection handling.

### 🧩 Overcoming C++98 Limitations
**Solution**: Built custom data structures and emphasized manual memory management.

### ⚡ Robust HTTP Parsing
**Solution**: Developed a finite-state machine parser supporting chunked encoding, multipart forms, and large requests.

### 🧪 Secure CGI Execution
**Solution**: Sandbox execution with strict environment isolation for **PHP** and **Python** CGI scripts.

### 📁 Scalable File Uploads
**Solution**: Stream uploads directly to disk instead of memory, avoiding DoS vulnerabilities.

### 🔄 Full HTTP Status Code Management
**Solution**: Built a clean system to generate appropriate responses for every client error, server error, or redirection.

### 🏗️ Dynamic Configuration System
**Solution**: Wrote a parser that validates input and provides rich error feedback.

---

## 👨‍💻 The Dream Team

<div align="center">
  <img src="https://github.com/Serghini04/WebServ/assets/webserv-team.png" alt="WebServ Team" width="700">
</div>

### 🧙‍♂️ Mehdi Serghini ([@Serghini04](https://github.com/Serghini04))
> *"The Request Whisperer"*

- 📥 HTTP request parsing
- 🔄 HTTP method implementation (GET, POST, DELETE)
- ✅ HTTP Status Code management
- 🔐 Secure session management
- 📦 Robust content handling (Content-Length, Chunked Transfer, Multipart Boundary)
- ⚡ Dynamic CGI (PHP + Python)

**Key Contributions**:
- Designed the HTTP parser based on a state machine
- Implemented safe, efficient file upload system
- Created full CGI integration for both PHP and Python
- Developed complete status code handling system
- Engineered session token system with expiration control

### 🏗️ Mohammed Al Morabit ([@mal-mora](https://github.com/mal-mora))
> *"The Architect"*

- 🏢 Server multi-instance management
- 📨 HTTP response generation
- 🔌 Low-level socket programming
- 🔄 Event-driven I/O core system

### 🧪 Hicham Driouch ([@hidriuoc](https://github.com/hidriuoc))
> *"The Configuration Alchemist"*

- ⚙️ Configuration file parsing
- 🔄 CGI execution system
- 🛣️ Routing and directory indexing

---

## 🔮 Future Enhancements

- 🔒 HTTPS support (SSL/TLS)
- 🚀 WebSocket real-time communication
- 🌐 Load balancing features
- 📊 Logging & monitoring dashboard
- 🧪 Unit and integration tests
- 🚦 Advanced rate-limiting for DDoS protection
- 🗄️ In-memory caching
- 🔄 Reverse proxy mode

---

## 📄 License

This masterpiece is licensed under the **MIT License** — giving you full freedom to create, adapt, and share!

<div align="center">
  <img src="https://github.com/Serghini04/WebServ/assets/end.png" alt="WebServ Footer" width="800">
  
  ### Made with ❤️ and countless cups of ☕ by Mehdi Serghini & team
</div>
