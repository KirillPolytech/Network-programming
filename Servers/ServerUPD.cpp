#include <iostream>
#include <fstream>
#include <winsock2.h>
#include <iostream>

#include "ServerTCP.cpp"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define BUFLEN 4096
#define PORT 8888

int main()
{
    Launch();
    return 0;
    
    WSADATA wsa;
    SOCKET server_socket = 0;
    sockaddr_in server, client;

    // initialise winsock.
    cout << "Initialising Winsock...\n";
    auto temp = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (temp != 0)
    {
        cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
        exit(EXIT_FAILURE);
    }
    cout << "Initialised.\n";

    // create a socket.
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        cout << "Could not create socket: " << WSAGetLastError() << "\n";
        exit(EXIT_FAILURE);
    }
    cout << "Socket created.\n";

    // Prepare the sockaddr_in structure.
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind socket.
    temp = bind(server_socket, reinterpret_cast<sockaddr*>(&server), sizeof(server));
    if (temp == SOCKET_ERROR)
    {
        cout << "Bind failed with error code: " << WSAGetLastError() << "\n";
        exit(EXIT_FAILURE);
    }
    cout << "Bind done.\n";

    while (true)
    {
        cout << "Waiting for data...\n";
        char message[BUFLEN] = {};

        // try to receive some data, this is a blocking call
        int slen = sizeof(sockaddr_in);
        int message_len = recvfrom(server_socket, message, BUFLEN, 0, reinterpret_cast<sockaddr*>(&client), &slen);
        if (message_len == SOCKET_ERROR)
        {
            cout << "recvfrom() failed with error code: " << WSAGetLastError() << "\n";
            exit(0);
        }

        // print details of the client/peer and the data received
        cout << "Received packet from " << inet_ntoa(client.sin_addr) << " " << ntohs(client.sin_port) << "\n";
        cout << "Data: " << message << "\n";

        cout << "Enter response (exit to stop server process): ";
        cin.getline(message, BUFLEN);

        // reply to the client with the same data
        temp = sendto(server_socket, message, strlen(message), 0, reinterpret_cast<sockaddr*>(&client), sizeof(sockaddr_in));
        if (temp == SOCKET_ERROR)
        {
            cout << "Sendto() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        if (strcmp(message, "exit") == 0)
        {
            cout << "Exiting server...\n";
            break;
        }
    }
}