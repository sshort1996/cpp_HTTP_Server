/*
1. Create a socket for listening on a specific port (e.g., port 5000)
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
#include <sys/types.h>
#include <winsock2.h>
#include <nlohmann/json.hpp>

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
    int socket_port = 5000;
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Bind to localhost
    server_address.sin_port = htons(socket_port); // Bind to port socket_port

    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    std::cout << "Socket bound to localhost on port " << socket_port << " successfully" << std::endl;

    // Listen for incoming connections
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR) { 
        std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl; 
        closesocket(server_socket); 
        WSACleanup(); 
        return 1; 
    }
    else{
        cout << "Listening on port " << socket_port << std::endl;
    }

    // loop over incoming connections
    while (true) {

        // create client side socket
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

        // Process data from the incoming request
        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            string request(buffer, bytes_received);

            // expect data in some 'pseudo json format eg. data={key1=value1, key2=value2, ...}
            string data;
            size_t start_pos = request.find("{"); // Find the start of the data
            size_t end_pos = request.find("}", start_pos)+1; // Find the end of the data
            
            if (start_pos != string::npos && end_pos != string::npos) {
                data = request.substr(start_pos, end_pos - start_pos);
                auto jsonData = nlohmann::json::parse(data);
                string givenName = jsonData["GivenName"].get<string>();
                cout << "Json formatted command line argument " << jsonData << std::endl;
                cout << "Fetched value from JSON: " << givenName << std::endl;

                // Construct the response based on the content of the request
                string response = "HTTP/1.1 200 OK\r\nContent-Length: " 
                    + to_string(3 + data.length()) 
                    + "\r\n\r\nHi " + givenName;

                const char* response_cstr = response.c_str(); // Convert std::string to const char*
                send(client_socket, response_cstr, strlen(response_cstr), 0);
            }
            
        } else {
            const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
            send(client_socket, response, strlen(response), 0);
        }
        // Wait for the client to receive the response
        // Add appropriate error handling for recv
        // char buffer[1024];
        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received < 0) {
            cout << "recv error, response not received";
        }

        // Close the client socket after the response is sent and received
        closesocket(client_socket);

        // Shutdown the socket to prevent further sends/receives
        // shutdown(client_socket, SD_SEND);
        // do {
        //     bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        // } while (bytes_received > 0);
        
    }

    // // Close the listening socket
    closesocket(server_socket);
    WSACleanup();

    return 0;
}
