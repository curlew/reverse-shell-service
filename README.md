<div align="center">

# Reverse Shell Service
[![Codacy grade](https://img.shields.io/codacy/grade/e9336e54ee254d0e994142b77bc3a68a?logo=codacy&style=flat-square)](https://app.codacy.com/gh/curlew/reverse-shell-service/dashboard)
![GitHub](https://img.shields.io/github/license/curlew/reverse-shell-service?style=flat-square)

</div>

## Compilation
```
$ cmake -B build -DSERVER_HOST="192.168.1.2" -DSERVER_PORT="4321"
$ cmake --build build --config Release
```

The following options can be set:
- `-DSERVER_HOST` (default: "localhost")
- `-DSERVER_PORT` (default: "1234")
- `-DSERVICE_NAME` (default: "svcname")
- `-DSERVICE_DESC` (default: none)
- `-DDISPLAY_NAME` (default: "Service Name")

## Installation

The service binary will install and automatically start itself when run with the `install` argument.
**The service cannot be installed without administrative priviledges. Run the binary in an elevated prompt or installation will fail.**
Exit code can then be determined by checking the value of `$LastExitCode`, in PowerShell, or `%ErrorLevel%`, in CMD.

| Exit Code | Cause |
| - | - |
| 0 | Installation succeeded |
| 1 | Error getting path to the service binary |
| 2 | Error opening a handle to the SCM |
| 3 | Error creating the service |
