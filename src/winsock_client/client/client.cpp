// TODO: Write a meaningful header

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#include "client_helpers.hpp"

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char** argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL,
        * ptr = NULL,
        hints;
    char rec_buffer[DEFAULT_BUFLEN];
    char out_buffer[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    const char* username = argv[2];

    // Validate the parameters
    if (argc != 3) {
        printf("Appropriate Params: %s, ip address (localhost for development), chat username\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send login message
    char login_buffer[128];
    sprintf_s(login_buffer, "%s connected to chat server!\n", username);
    iResult = send(ConnectSocket, login_buffer, (int)strlen(login_buffer), 0);
    if (iResult != SOCKET_ERROR)
        printf(login_buffer);

    while (1)
    {
        iResult = recv(ConnectSocket, rec_buffer, recvbuflen, 0);
        if (iResult > 0)
            printf("%s", rec_buffer);

        //sprintf_s(out_buffer, "%s: This is a test message!\n", username);
        //iResult = send(ConnectSocket, out_buffer, (int)strlen(out_buffer), 0);
        if (iResult != SOCKET_ERROR)
            printf("%s", out_buffer);
        // TODO: Add a meaningful termination condition
        // break;
    }

    // cleanup
    iResult = shutdown(ConnectSocket, SD_SEND);
    freeaddrinfo(result);
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}