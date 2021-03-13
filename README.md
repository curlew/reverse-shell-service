<div align="center">

# Reverse Shell Service
![Codacy grade](https://img.shields.io/codacy/grade/7355bce26a454b16953531de1b831d75?style=flat-square&logo=codacy)
![GitHub](https://img.shields.io/github/license/curlew/reverse-shell-service?style=flat-square)

</div>

## Compilation
`SERVER_HOST` and `SERVER_PORT` default to `localhost` and `1234`, respectively.
```
$ cmake -B build -DSERVER_HOST="192.168.1.2" -DSERVER_PORT="4321"
$ cmake --build build
```

## Installation

The service binary will install and automatically start itself when run with the `install` argument.
**The service cannot be installed without administrative priviledges. Run the binary in an elevated prompt or installation will fail.**
Exit code can then be determined by checking the value of `$LastExitCode`, in PowerShell, or `%ErrorLevel%`, in CMD.

| Exit Code | Cause |
| - | - |
| 0 | Installation succeeded |
| 1\* | Error getting path to the service binary |
| 2 | Error opening a handle to the SCM |
| 3 | Error creating the service |

\*: May indicate a bug. Please open an issue, specifying the actual location of the service binary.  
