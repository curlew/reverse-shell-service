#ifndef GUARD_7BC1A2847D324ECB8089708185E04030
#define GUARD_7BC1A2847D324ECB8089708185E04030

#include <windows.h>

extern SERVICE_STATUS        g_svc_status;
extern SERVICE_STATUS_HANDLE g_svc_status_handle;
extern HANDLE                g_stop_event;

void WINAPI svc_main(DWORD, LPWSTR *);

#endif // GUARD_7BC1A2847D324ECB8089708185E04030
