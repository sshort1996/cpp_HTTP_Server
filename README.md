Lets build a HTTP server in C++. First we need to know what an HTTP server is and what it does.

  We'll step through this pseudocode and build the server piece by piece

  

```plaintext

1. Create a socket for listening on a specific port (e.g., port 8080)

2. Bind the socket to the port

3. Listen for incoming connections

4. Accept incoming connections

5. Receive the HTTP request from the client

6. Parse the HTTP request to extract the requested resource (e.g., URL)

7. Check the requested resource and generate an appropriate HTTP response

8. Send the HTTP response back to the client

9. Close the connection

10. Repeat steps 4-9 to handle multiple client connections

11. Close the listening socket when done

  

Pseudocode Example:

  

// Create a socket

socket = create_socket()

  

// Bind the socket to a specific port

bind_socket(socket, 8080)

  

// Listen for incoming connections

listen(socket)

  

while true:

    // Accept incoming connection

    client_socket = accept_connection(socket)

  

    // Receive HTTP request from client

    request = receive_request(client_socket)

  

    // Parse the request to extract the requested resource

    requested_resource = parse_request(request)

  

    // Check the requested resource and generate an appropriate response

    response = generate_response(requested_resource)

  

    // Send the HTTP response back to the client

    send_response(client_socket, response)

  

    // Close the connection

    close_connection(client_socket)

  

// Close the listening socket

close_socket(socket)

```

  
## Creating a Socket
First, creating a socket. We'll need the ``<sys/types.h>`` and ``<sys/socket.h>`` header files in our env and then will create a socket as:

  

```

#include <iostream>

#include <sys/types.h>

#include <sys/socket.h>

  

int main() {

    // Create a socket

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == -1) {

        std::cerr << "Error creating socket" << std::endl;

        return 1;

    }

    std::cout << "Socket created successfully" << std::endl;

    // Close the socket

    close(server_socket);

    return 0;

}

```

  

However this is Unix specific, so since we're currently stuck on windows we'll use `winsock2`

  

```cpp

#include <iostream>

#include <sys/types.h> //socket stuff

// #include <sys/socket.h>

#include <winsock2.h>

  

using namespace std;

  

int main() {

    // Initialize Winsock

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {

        std::cerr << "WSAStartup failed" << std::endl;

        return 1;

    }

  

    // Create a socket

    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  

    if (server_socket == INVALID_SOCKET) {

        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;

        WSACleanup();

        return 1;

    }

  

    std::cout << "Socket created successfully" << std::endl;

  

    // Close the socket

    closesocket(server_socket);

    WSACleanup();

  

    return 0;

}

```

  

Compiling as normal we get these errors

```

$ g++ helloHTTP.cpp -o http

C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/13.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:\Users\kultu\AppData\Local\Temp\ccXvdi5X.o:helloHTTP.cpp:(.text+0x25): undefined reference to `__imp_WSAStartup'

C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/13.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:\Users\kultu\AppData\Local\Temp\ccXvdi5X.o:helloHTTP.cpp:(.text+0x7c): undefined reference to `__imp_socket'

C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/13.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:\Users\kultu\AppData\Local\Temp\ccXvdi5X.o:helloHTTP.cpp:(.text+0xb2): undefined reference to `__imp_WSAGetLastError'

C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/13.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:\Users\kultu\AppData\Local\Temp\ccXvdi5X.o:helloHTTP.cpp:(.text+0xd7): undefined reference to `__imp_WSACleanup'

C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/13.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:\Users\kultu\AppData\Local\Temp\ccXvdi5X.o:helloHTTP.cpp:(.text+0x11c): undefined reference to `__imp_closesocket'

C:/msys64/ucrt64/bin/../lib/gcc/x86_64-w64-mingw32/13.2.0/../../../../x86_64-w64-mingw32/bin/ld.exe: C:\Users\kultu\AppData\Local\Temp\ccXvdi5X.o:helloHTTP.cpp:(.text+0x125): undefined reference to `__imp_WSACleanup'

collect2.exe: error: ld returned 1 exit status

```

  

due to a linking error in the compilation, we must link against the windows socket API during compilation as:

```

g++ helloHTTP.cpp -o http -lws2_32

```
More details on [[Linkers]]

Which compiles correctly and creates a socket as expected.

## Binding socket to a port
Binding a socket to a port in networking refers to associating a specific network port on a machine with a socket. When a socket is bound to a port, it means that the socket is configured to listen for incoming network connections on that particular port.

We bind the socket created above to a specific socket (in this case localhost 8080) as follows:
```
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Bind to localhost
    server_address.sin_port = htons(8080); // Bind to port 8080
  
    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
  
    std::cout << "Socket bound to localhost on port 8080 successfully" << std::endl;
  
    closesocket(server_socket);
    WSACleanup();
```

## Listening for incoming requests on this socket

To accept incoming requests we need to first tell the socket to listen.
```cpp
// Listen for incoming connections
if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) { 
	std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl; 
	closesocket(server_socket); 
	WSACleanup(); 
	return 1; 
}
```
We then accept incoming requests in a loop and respond "Hello World" to them
```cpp
    while (true) {
        SOCKET client_socket;
        sockaddr_in client_address;
        int client_address_size = sizeof(client_address);
  
        client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_size);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }
  
        std::cout << "Incoming connection accepted" << std::endl;
  
        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        send(client_socket, response, strlen(response), 0);
  
        closesocket(client_socket);
    }
```
The problem here is we are note closing the client socket correctly. It will work once but then susequent requests will lead to undefined behaviour. To fix this we shutdown the socket after responding, and then receive any remaining data before closing the socket altogether. Corrected code here

```cpp
    while (true) {
        SOCKET client_socket;
        sockaddr_in client_address;
        int client_address_size = sizeof(client_address);
  
        client_socket = accept(server_socket, (sockaddr*)&client_address, &client_address_size);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_socket);
            WSACleanup();
            return 1;
        }
  
        std::cout << "Incoming connection accepted" << std::endl;
  
        const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        send(client_socket, response, strlen(response), 0);
        // Shutdown the socket to prevent further sends/receives
        shutdown(client_socket, SD_SEND);
  
        // Receive any remaining data from the client
        char buffer[1024];
        int bytes_received;
        do {
            bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        } while (bytes_received > 0);
  
        // Close the client socket
        closesocket(client_socket);
    }
```
The server can now handle multiple connections without having undefined behaviour.