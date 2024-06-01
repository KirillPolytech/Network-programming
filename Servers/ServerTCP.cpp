#include <cstdio>
#include <fstream>
#include <iostream>
#include <winsock2.h>

using namespace std;

#define BUFLEN 4096
#define PORT 8888

static int Launch()
{
    int rc, i = 0;
    WSADATA wsd;
    SOCKET _socket1, _socket2;
    ofstream _file;
    FILE* _file2;
    size_t size, symbols;

    const char* fileName = "ab.txt";

    // Инициализация использования сетевых функций Winsock.
    WSAStartup(0X0101, &wsd);
    
    // Создаем сокет для работы с протоколом IPv4 (AF_INET) и типом потока (SOCK_STREAM).
    _socket2 = socket(AF_INET, SOCK_STREAM, 0); // tcp

    sockaddr_in myCom;
    // Семейство адресов (AF_INET).
    myCom.sin_family = AF_INET; // семейство адресов (AF_INET).
    // Порт 1200.
    myCom.sin_port = htons(PORT);
    // Адрес сервера INADDR_ANY, сервер будет слушать все доступные сетевые интерфейсы.
    myCom.sin_addr.S_un.S_addr = INADDR_ANY;

    // Привязывает сокет к указанной структуре с адресом и портом. Если привязка прошла успешно, rc будет равно 0.
    // Не работает при использовагии UPD.
    rc = bind(_socket2, reinterpret_cast<sockaddr*>(&myCom), sizeof(myCom));
    if (rc != 0)
        cout << "RC error" << "\n";

    // Устанавливает сокет в режим прослушивания, готовый принимать входящие соединения с максимальным количеством ожидающих соединений равным 5.
    // Не работает при использовагии UPD.
    listen(_socket2, 5);

    // Ожидает входящее подключение на прослушиваемом сокете и принимает его.
    // Если возвращенный сокет _socket1 равен -1, то выводится ошибка.
    // Не работает при использовагии UPD.
    _socket1 = accept(_socket2, nullptr, nullptr);
    if (_socket1 == -1)
        cout << "accept error" << "\n";
    
    while (true)
    {
        char buf[BUFLEN]{0};
        int r = recv(_socket1, buf, sizeof(buf), 0);

        if (r < 0)
        {
            // Вывод кода ошибки.
            //cout << "Error: " << WSAGetLastError() << "\n";
            //break;
        }

        if (buf[0] == '1')
        {
            recv(_socket1, buf, sizeof(buf), 0);

            _file.open(fileName);
            _file << buf;
            _file.close();

            cout << "Message:" << buf << "\n";
            cout << "receive bytes: " << r << "\n";
            // Передача ответа клиенту.
            send(_socket1, "200", sizeof(buf), 0);
        }
        else
        {
            _file2 = _fsopen("ab.txt", "r+", _SH_DENYNO);
            if (_file2 == nullptr)
            {
                cout << "Open file error\n";
                break;
            }

            char buff[BUFLEN]{0};
            while (!feof(_file2)) // Передаем файл частями (сколько помещается в буфере).
            {
                symbols = fread(buff, sizeof(char), sizeof(buff), _file2);
                size = ftell(_file2);

                printf("read symbols: %d, part: %d, pos: %ld \n", symbols, i++, size);

                if (symbols != 0)
                {
                    send(_socket1, buff, BUFLEN * sizeof(char), 0);

                    cout << "Send message: " << buff << "\n";
                }
                else
                {
                    send(_socket1, " ", sizeof(char), 0);

                    cout << "Send message: null" << "\n";
                    break;
                }
            }
        }
    }

    shutdown(_socket1, 2);
    WSACleanup();

    return 0;
}
/*
// Если данных нет.
if (r == 0)
{
    break;
}
*/