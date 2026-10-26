
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

SOCKET clients[2];
char names[2][50];

DWORD WINAPI handle_client(LPVOID arg)
{
    int index = *(int *)arg;
    char buffer[BUFFER_SIZE];

    while (1)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytes = recv(clients[index], buffer,
                         sizeof(buffer) - 1, 0);

        if (bytes <= 0)
            break;

        buffer[bytes] = '\0';

        printf("%s: %s\n", names[index], buffer);

        // Send the message to the other client
        int other = 1 - index;

        if (clients[other] != INVALID_SOCKET)
        {
            char message[BUFFER_SIZE + 100];

            snprintf(message, sizeof(message),
                     "%s: %s", names[index], buffer);

            send(clients[other], message,
                 (int)strlen(message), 0);
        }
    }

    printf("%s disconnected.\n", names[index]);
    closesocket(clients[index]);
    clients[index] = INVALID_SOCKET;

    return 0;
}

int main(void)
{
    WSADATA wsa;
    SOCKET server;
    struct sockaddr_in address;
    int address_length = sizeof(address);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSAStartup failed.\n");
        return 1;
    }

    server = socket(AF_INET, SOCK_STREAM, 0);

    if (server == INVALID_SOCKET)
    {
        printf("Socket creation failed.\n");
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server, (struct sockaddr *)&address,
             sizeof(address)) == SOCKET_ERROR)
    {
        printf("Bind failed. Port may already be in use.\n");
        closesocket(server);
        WSACleanup();
        return 1;
    }

    listen(server, 2);

    clients[0] = INVALID_SOCKET;
    clients[1] = INVALID_SOCKET;

    printf("Server waiting for 2 clients...\n");

    for (int i = 0; i < 2; i++)
    {
        clients[i] = accept(server,
                            (struct sockaddr *)&address, &address_length);

        if (clients[i] == INVALID_SOCKET)
        {
            printf("Client connection failed.\n");
            closesocket(server);
            WSACleanup();
            return 1;
        }

        int bytes = recv(clients[i], names[i],
                         sizeof(names[i]) - 1, 0);

        if (bytes <= 0)
        {
            closesocket(clients[i]);
            clients[i] = INVALID_SOCKET;
            i--;
            continue;
        }

        names[i][bytes] = '\0';
        printf("%s connected!\n", names[i]);

        char welcome[100];
        snprintf(welcome, sizeof(welcome),
                 "Welcome %s!", names[i]);

        send(clients[i], welcome,
             (int)strlen(welcome), 0);
    }

    printf("\nBoth clients connected!\n");
    printf("Chat started...\n\n");

    HANDLE threads[2];
    int ids[2] = {0, 1};

    for (int i = 0; i < 2; i++)
        threads[i] = CreateThread(NULL, 0,
                                  handle_client, &ids[i], 0, NULL);

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    for (int i = 0; i < 2; i++)
        CloseHandle(threads[i]);

    closesocket(server);
    WSACleanup();

    return 0;
}
