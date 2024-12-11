#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define MAX_CLIENTS 64

int __cdecl main(void)
{
    WSADATA wsaData;
    int iResult;
    int ClientCount = 0;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSockets[MAX_CLIENTS];

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    printf("Chat server online!\n");
    // Persistently loop and listen for new clients
    // and then receive packets, then broadcast them to the others

    while (1)
    {
        iResult = listen(ListenSocket, SOMAXCONN);
        SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket != INVALID_SOCKET) 
        {
            ClientSockets[ClientCount] = ClientSocket;
            ClientCount++;
            printf("Client connected!\n");
        }

        for (int i = 0; i < ClientCount; i++)
        {
            iResult = recv(ClientSockets[i], recvbuf, recvbuflen, 0);
            if (iResult == SOCKET_ERROR)
            {
                // Client disconnected
                for (int j = i; j < ClientCount; j++)
                {
                    ClientSockets[j] = ClientSockets[j + 1];
                }
                ClientCount--;
                for (int j = 0; j < ClientCount; j++)
                    send(ClientSockets[j], "Client has disconnected.\n", iResult, 0);
            }
            else
            {
                // Send the packet to other clients
                printf("Message received\n");
                for (int j = 0; j < ClientCount; j++)
                {
                    //if (i == j)
                    //    continue;

                    send(ClientSockets[j], recvbuf, iResult, 0);
                }
            }
        }

        // Add some termination condition here, probably button input
        // break;
    }

    // Close all client connections
    for (int i = 0; i < ClientCount; i++)
        shutdown(ClientSockets[i], SD_SEND);

    // Close server socket
    closesocket(ListenSocket);

    WSACleanup();

    return 0;
}