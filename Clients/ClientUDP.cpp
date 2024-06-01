#pragma warning(disable:4996)

#include <iostream>
#include <fstream>
#include <WinSock2.h>
#include "ClientTCP.cpp"

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define SERVERADRESS "127.0.0.1"  // or "localhost" - ip address of UDP server
#define BUFLEN 4096  // max length of answer
#define PORT 8888  // the port on which to listen for incoming data

int main()
{
    Launch();
    return 0;
    
    WSADATA ws;
    SOCKET client_socket;
    sockaddr_in server;

    // initialize winsock.
    cout << "Initialising Winsock...\n";
    auto temp = WSAStartup(MAKEWORD(2, 2), &ws);
    if (temp != 0)
    {
        cout << "Failed. Error Code: " << WSAGetLastError() << "\n";
        exit(EXIT_FAILURE);
    }
    cout << "Initialised.\n";

    // Create socket.
    client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (client_socket == SOCKET_ERROR)
    {
        cout << "socket() failed with error code: " << WSAGetLastError() << "\n";
        exit(EXIT_FAILURE);
    }

    // setup address structure
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.S_un.S_addr = inet_addr(SERVERADRESS);
    
    while (true)
    {
        char message[BUFLEN];
        cout << "Enter message: ";
        cin.getline(message, BUFLEN);

        // send the message
        auto temp2 = sendto(client_socket, message, strlen(message), 0, (sockaddr*)&server, sizeof(sockaddr_in));
        if (temp2 == SOCKET_ERROR)
        {
            cout << "sendto() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        // Receive a reply and print it.
        // Clear the answer by filling null, it might have previously received data.
        char answer[BUFLEN] = {};

        // Try to receive some data, this is a blocking call.
        int slen = sizeof(sockaddr_in);
        int answer_length = recvfrom(client_socket, answer, BUFLEN, 0, (sockaddr*)&server, &slen);
        
        if (answer_length == SOCKET_ERROR)
        {
            cout << "Recvfrom() failed with error code: " << WSAGetLastError() << "\n";
            exit(EXIT_FAILURE);
        }

        cout << "Server's response: " << answer << "\n";
    }
}