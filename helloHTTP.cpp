/*
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
*/
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

    // Bind the socket to a specific port
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

    // // Listen for incoming connections
    // listen(socket)

    // while true:
    //     // Accept incoming connection
    //     client_socket = accept_connection(socket)

    //     // Receive HTTP request from client
    //     request = receive_request(client_socket)

    //     // Parse the request to extract the requested resource
    //     requested_resource = parse_request(request)

    //     // Check the requested resource and generate an appropriate response
    //     response = generate_response(requested_resource)

    //     // Send the HTTP response back to the client
    //     send_response(client_socket, response)

    //     // Close the connection
    //     close_connection(client_socket)

    // // Close the listening socket
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
