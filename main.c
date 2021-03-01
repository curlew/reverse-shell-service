#include <windows.h>

#define SERVICE_NAME TEXT("svcname")
#define DISPLAY_NAME TEXT("Service Name")

SERVICE_STATUS g_svc_status = {
    .dwServiceType             = SERVICE_WIN32_OWN_PROCESS,
    .dwServiceSpecificExitCode = 0,
    .dwControlsAccepted        = 0 // updated at the first call to report_status with a non-pending current state
};
SERVICE_STATUS_HANDLE g_svc_status_handle;
HANDLE g_stop_event;

void WINAPI svc_main(DWORD, LPWSTR *);
void WINAPI ctrl_handler(DWORD);
void report_status(DWORD, DWORD, DWORD);

int wmain(int argc, wchar_t *argv[], wchar_t *envp[]) {
    SERVICE_TABLE_ENTRYW dispatch_table[] = {{SERVICE_NAME, svc_main},
                                             {NULL, NULL}};

    // returns when the service has stopped; process should terminate then
    if (!StartServiceCtrlDispatcherW(dispatch_table)) {
        return 1; // TODO
    }
}

void WINAPI svc_main(DWORD argc, LPWSTR *argv) {
    // this handle doesn't need to be closed
    g_svc_status_handle = RegisterServiceCtrlHandlerW(SERVICE_NAME, ctrl_handler);
    if (!g_svc_status_handle) {
        return;
    }

    report_status(SERVICE_START_PENDING, NO_ERROR, 500);

    // there's no risk of receiving a stop signal here before g_stop_event has
    // been created, as, by default, only SERVICE_CONTROL_INTERROGATE is accepted.
    // this is updated at the first call to report_status with SERVICE_RUNNING

    g_stop_event = CreateEventW(NULL,  // default security attributes
                                TRUE,  // manually reset
                                FALSE, // initial state is nonsignalled
                                NULL); // unnamed
    if (!g_stop_event) {
        report_status(SERVICE_STOPPED, GetLastError(), 0);
        return;
    }

    report_status(SERVICE_RUNNING, NO_ERROR, 0);

    WaitForSingleObject(g_stop_event, INFINITE);
    // SERVICE_STOP_PENDING has already been reported by ctrl_handler upon receiving
    // stop signal
    report_status(SERVICE_STOPPED, NO_ERROR, 0);
}

void WINAPI ctrl_handler(DWORD ctrl) {
    switch (ctrl) {
    case SERVICE_CONTROL_STOP:
        report_status(SERVICE_STOP_PENDING, NO_ERROR, 500);
        SetEvent(g_stop_event);
        break;
    }
}

void report_status(DWORD current_state, DWORD win32_exit_code, DWORD wait_hint) {
    static DWORD checkpoint = 1;

    g_svc_status.dwCurrentState  = current_state;
    g_svc_status.dwWin32ExitCode = win32_exit_code;
    g_svc_status.dwWaitHint      = wait_hint;

    // service shouldn't accept SERVICE_CONTROL_STOP while starting
    g_svc_status.dwControlsAccepted = current_state == SERVICE_START_PENDING
                                      ? 0 : SERVICE_ACCEPT_STOP;

    // checkpoint should be 0 if the service isn't in a pending state
    if (current_state != SERVICE_RUNNING || current_state != SERVICE_STOPPED ||
        current_state != SERVICE_PAUSED) {
        g_svc_status.dwCheckPoint = 0;
    } else {
        g_svc_status.dwCheckPoint = checkpoint++;
    }

    SetServiceStatus(g_svc_status_handle, &g_svc_status);
}
