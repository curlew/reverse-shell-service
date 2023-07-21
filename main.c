#include "service.h"
#include <stdlib.h>
#include <windows.h>

int wmain(int argc, wchar_t *argv[], wchar_t *envp[]) {
    (void)argc;
    (void)envp;

    if (lstrcmpiW(argv[1], L"install") == 0) {
        wchar_t self_path[MAX_PATH];
        // "FileName" here is referring to a fully qualified name (path)
        if (!GetModuleFileNameW(NULL, self_path, _countof(self_path))) {
            return 1;
        }

        SC_HANDLE scm = OpenSCManager(NULL, NULL, GENERIC_WRITE);
        if (!scm) {
            return 2;
        }

        SC_HANDLE service =
            CreateService(scm, SERVICE_NAME, DISPLAY_NAME,
                          SERVICE_ALL_ACCESS,
                          SERVICE_WIN32_OWN_PROCESS,
                          SERVICE_AUTO_START,
                          SERVICE_ERROR_NORMAL,
                          self_path,
                          NULL,
                          NULL,
                          NULL,
                          NULL,
                          NULL);
        if (!service) {
            CloseServiceHandle(scm);
            return 3;
        }

        if (SERVICE_DESC[0] != L'\0') {
            SERVICE_DESCRIPTIONW desc;
            desc.lpDescription = SERVICE_DESC;
            ChangeServiceConfig2W(service, SERVICE_CONFIG_DESCRIPTION, &desc);
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
