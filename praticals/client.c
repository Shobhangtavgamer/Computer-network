
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

SOCKET sock;

DWORD WINAPI receive_messages(LPVOID arg)
{
     char buffer[BUFFER_SIZE];

     while (1)
     {
          memset(buffer, 0, sizeof(buffer));

          int bytes = recv(sock, buffer,
                           sizeof(buffer) - 1, 0);

          if (bytes <= 0)
          {
               printf("\nDisconnected from server.\n");
               break;
          }

          buffer[bytes] = '\0';
          printf("\n%s\n", buffer);
          printf("> ");
          fflush(stdout);
     }

     return 0;
}

int main(void)
{
     WSADATA wsa;
     struct sockaddr_in server;
     char name[50];
     char message[BUFFER_SIZE];

     if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
     {
          printf("WSAStartup failed.\n");
          return 1;
     }

     sock = socket(AF_INET, SOCK_STREAM, 0);

     if (sock == INVALID_SOCKET)
     {
          printf("Socket creation failed.\n");
          WSACleanup();
          return 1;
     }

     server.sin_family = AF_INET;
     server.sin_port = htons(PORT);
     server.sin_addr.s_addr = inet_addr("127.0.0.1");

     if (connect(sock, (struct sockaddr *)&server,
                 sizeof(server)) == SOCKET_ERROR)
     {
          printf("Unable to connect. Start the server first.\n");
          closesocket(sock);
          WSACleanup();
          return 1;
     }

     printf("Enter your name: ");
     fgets(name, sizeof(name), stdin);
     name[strcspn(name, "\r\n")] = '\0';

     send(sock, name, (int)strlen(name), 0);

     char welcome[BUFFER_SIZE] = {0};
     int bytes = recv(sock, welcome, sizeof(welcome) - 1, 0);

     if (bytes <= 0)
     {
          printf("Server disconnected.\n");
          closesocket(sock);
          WSACleanup();
          return 1;
     }

     welcome[bytes] = '\0';
     printf("%s\n", welcome);

     HANDLE thread = CreateThread(NULL, 0,
                                  receive_messages, NULL, 0, NULL);

     if (thread == NULL)
     {
          printf("Unable to create receiver thread.\n");
          closesocket(sock);
          WSACleanup();
          return 1;
     }

     while (1)
     {
          printf("> ");
          fflush(stdout);

          if (fgets(message, sizeof(message), stdin) == NULL)
               break;

          message[strcspn(message, "\r\n")] = '\0';

          if (strcmp(message, "exit") == 0)
               break;

          if (strlen(message) == 0)
               continue;

          if (send(sock, message,
                   (int)strlen(message), 0) == SOCKET_ERROR)
          {
               printf("Send failed.\n");
               break;
          }
     }

     shutdown(sock, SD_BOTH);
     closesocket(sock);
     WaitForSingleObject(thread, 1000);
     CloseHandle(thread);
     WSACleanup();

     return 0;
}
