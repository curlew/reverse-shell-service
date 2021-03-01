#include "service.h"
#include <stdlib.h>
#include <windows.h>

int wmain(int argc, wchar_t *argv[], wchar_t *envp[]) {
    if (lstrcmpiW(argv[1], L"install") == 0) {
        wchar_t self_path[MAX_PATH];
        // "FileName" here is referring to a fully qualified name (path)
        if (!GetModuleFileNameW(NULL, self_path, _countof(self_path))) {
            return 1;
        }

        SC_HANDLE scm = OpenSCManager(NULL, NULL, GENERIC_WRITE);
        if (!scm) { return 2; }

        SC_HANDLE service =
            CreateService(scm, SERVICE_NAME, DISPLAY_NAME,
                          SERVICE_ALL_ACCESS,        // desired access
                          SERVICE_WIN32_OWN_PROCESS, // service type
                          SERVICE_AUTO_START,        // start type
                          SERVICE_ERROR_NORMAL,      // error control type
                          self_path,                 // path to executable
                          NULL,                      // no load ordering group
                          NULL,                      // no tag identifier
                          NULL,                      // no dependencies
                          NULL,                      // LocalSystem account
                          NULL);                     // no password
        if (!service) {
            CloseServiceHandle(scm);
            return 3;
        }

        StartServiceW(service, 0, NULL);

        CloseServiceHandle(service);
        CloseServiceHandle(scm);
        return 0;
    }

    /* if argv[1] isn't "install", program is probably being started by the SCM.
       all initialisation is deferred until the service entry point, as the
       SERVICE_START_PENDING status can be reported then */

    SERVICE_TABLE_ENTRYW dispatch_table[] = {{SERVICE_NAME, svc_main},
                                             {NULL, NULL}};

    // returns when the service has stopped; process should terminate then
    if (!StartServiceCtrlDispatcherW(dispatch_table)) {
        return 1; // TODO
    }
}
