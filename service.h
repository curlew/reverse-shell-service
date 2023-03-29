#ifndef SERVICE_H
#define SERVICE_H

#include <windows.h>

extern SERVICE_STATUS        g_svc_status;
extern SERVICE_STATUS_HANDLE g_svc_status_handle;
extern HANDLE                g_stop_event;

void WINAPI svc_main(DWORD, LPWSTR *);

#endif // SERVICE_H
