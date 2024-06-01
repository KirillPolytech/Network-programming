#pragma warning(disable:4996)

#include <cstdio>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

#pragma comment(lib, "Ws2_32.lib")

#define SERVERADRESS "127.0.0.1"  // or "localhost" - ip address of UDP server
#define BUFLEN 4096  // max length of answer
#define PORT 8888  // the port on which to listen for incoming data

static int Launch()
{
    size_t size, symbols;
    int rc;
    char read_buffer[BUFLEN]; // Буффер для считывания передаваемого файла.
    char receive_buffer[BUFLEN]{0}; // Буффер для приема сообщений сервера.
    WSADATA wsd;
    FILE* file = nullptr;
    SOCKET _socket;
    sockaddr_in peer;

    WSAStartup(0X0101, &wsd);
    _socket = socket(AF_INET, SOCK_STREAM, 0); // tcp

    peer.sin_family = AF_INET;
    peer.sin_port = htons(PORT);
    peer.sin_addr.S_un.S_addr = inet_addr(SERVERADRESS);

    rc = connect(_socket, reinterpret_cast<sockaddr*>(&peer), sizeof(peer));

    if (rc < 0)
        cout << "error: " << WSAGetLastError() << "\n";

    while (true)
    {
        string str = "";
        cout << "Enter 1 to send file" << "\n";
        cout << "Enter 2 to receive file" << "\n";
        cout << "Enter 3 to exit" << "\n";

        cin >> str;
        if (str == "1")
        {
            send(_socket, "1", sizeof(char), 0);

            fopen_s(&file, "NewFile.txt", "rb");
            // Передаем файл частями (сколько помещается в буфере).
            while (!feof(file))
            {
                symbols = fread(read_buffer, 1, sizeof(read_buffer), file);
                size = ftell(file);

                printf("read symbols: %d, pos: %ld \n", symbols, size);

                if (symbols != 0)
                    send(_socket, read_buffer, symbols * sizeof(char), 0);

                // Прием ответа сервера.
                recv(_socket, receive_buffer, sizeof(receive_buffer), 0);
                printf("Server asnwer: %s\n", receive_buffer);
            }

            fclose(file);
        }
        else if (str == "2")
        {
            send(_socket, "2", sizeof(char), 0);

            recv(_socket, receive_buffer, sizeof(receive_buffer), 0);

            cout << "Received file: " << receive_buffer << "\n";
        }

        if (str == "3")
        {
            cout << "Exit\n";
            break;
        }
    }

    fclose(file);
    shutdown(_socket, 2);
    WSACleanup();

    return 0;
}
