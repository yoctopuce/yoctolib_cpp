////////////////////////////////////////////////////////////////////////////////
//
//                Simple periodic text-messaging TCP/IP server
//
//                    for use with Yoctopuce USB sensors
//
////////////////////////////////////////////////////////////////////////////////

// Detect compiler
#if defined(_WIN32)
#define WINDOWS_API
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#undef UNICODE
#define _MBCS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <tchar.h>
#include <iphlpapi.h>
#define SAFE_SPRINTF sprintf_s
#define strdup _strdup

#else

#if defined(__linux__)
#define LINUX_API
#elif defined(__APPLE__)
#define OSX_API
#endif
#define _REENTRANT
#include <unistd.h>
typedef int SOCKET;
#define INVALID_SOCKET -1
#define closesocket(s) close(s)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdio.h>
#define SAFE_SPRINTF snprintf
#endif

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "yocto_api.h"

int ServerMain(void);

#ifdef WINDOWS_API
void InstallService(int install, const char *args);
#endif

// Default settings

#define DEFAULT_PORTNO  4455
#define DEFAULT_FREQ    "30/m"

typedef struct {
  unsigned    portno;
  const char  *freq;
  unsigned    runAsDaemon: 1;
  unsigned    runMore: 1;
  unsigned    encodeNMEA: 1;
} global_parameter;
global_parameter Globalp;

////////////////////////////////////////////////////////////////////////////////
//
//                      Common argument parsing code
//
////////////////////////////////////////////////////////////////////////////////

// Argument parsing
static void Usage(int argc, char *argv[])
{
  fprintf(stderr, "Usage:\n");
#ifdef WINDOWS_API
  fprintf(stderr, "%s [-i|-u] [-n] [-p <port>] [-f <freq>]\n", argv[0]);
  fprintf(stderr, "  -i        : install as an auto-start Windows service\n");
  fprintf(stderr, "  -u        : uninstall as a service\n");
#else
  fprintf(stderr, "%s [-d] [-n] [-p <port>] [-f <freq>\n", argv[0]);
  fprintf(stderr, "  -d        : run as a Unix daemon\n");
#endif
  fprintf(stderr, "  -n        : append NMEA checksum to each line\n");
  fprintf(stderr, "  -p <port> : select TCP/IP port on which the server runs\n");
  fprintf(stderr, "  -f <freq> : select frequency (eg. 1/s, 20/m, 12/m, 30/h, ...) \n");
  exit(1);
}

static int ParseArguments(int argc, char *argv[])
{
  int         i;
#ifdef WINDOWS_API
  int         install = 0;
#endif

  memset(&Globalp, 0, sizeof(Globalp));
  Globalp.runMore = 1;
  Globalp.portno = DEFAULT_PORTNO;
  Globalp.freq = DEFAULT_FREQ;

  for (i = 1; i < argc; i++) {
    if (argv[i][0] != '-' || strlen(argv[i]) > 2) {
      fprintf(stderr, "%s: unknown option %s\n", argv[0], argv[i]);
      Usage(argc, argv);
    }
    switch(argv[i][1]) {
    case 'f':
      i++;
      if (i < argc) {
        Globalp.freq = strdup(argv[i]);
      } else {
        fprintf(stderr, "%s: -f option requires an argument (frequency)\n", argv[0]);
        Usage(argc, argv);
      }
      break;
    case 'p':
      i++;
      if (i < argc) {
        Globalp.portno = atoi(argv[i]);
      } else {
        fprintf(stderr, "%s: -p option requires an argument (port number)\n", argv[0]);
        Usage(argc, argv);
      }
      break;
    case 'd':
      Globalp.runAsDaemon = 1;
      break;
    case 'n':
      Globalp.encodeNMEA = 1;
      break;
#ifdef WINDOWS_API
    case 'i':
      install = 1;
      break;
    case 'u':
      InstallService(0, "");
      exit(0);
#endif
    default:
      fprintf(stderr, "%s: unknown option %s\n", argv[0], argv[i]);
      Usage(argc, argv);
    }
  }

#ifdef WINDOWS_API
  if(install) {
    char allargs[1024];
    char *p = allargs;
    SAFE_SPRINTF(allargs, 1024, " -d -p %d -f %s", Globalp.portno, Globalp.freq);
    if(Globalp.encodeNMEA) {
      strcat(allargs, " -n");
    }
    printf("install with args [%s]\n", allargs);
    InstallService(1, allargs);
    exit(0);
  }
#endif

  return 1;
}

////////////////////////////////////////////////////////////////////////////////
//
//            OS-specific startup code (including service/daemon mode)
//
////////////////////////////////////////////////////////////////////////////////

#ifdef WINDOWS_API
// Windows version

#define SERVICE_NAME	"ySensorServer"

SERVICE_STATUS          ServiceStatus;
SERVICE_STATUS_HANDLE   ServiceStatusHandle;
HANDLE                  ServiceStopEvent = NULL;

void  WinError(const char *msg, DWORD err)
{
  LPTSTR lpMsgBuf;
  FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPTSTR) &lpMsgBuf,
                0, NULL );
  fprintf(stderr, "%s (%d:%s)", msg, err, lpMsgBuf);
}

void InstallService(int install, const char *args)
{
  SC_HANDLE           SCman, service;
  TCHAR               path[2048];

  if(!GetModuleFileNameA(NULL, path, sizeof(path))) {
    WinError("Cannot install service", GetLastError());
    return;
  }
  strcat_s(path, sizeof(path), args);
  SCman = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
  if(!SCman) {
    WinError("OpenSCManager failed", GetLastError());
    return;
  }
  service = OpenService(SCman, SERVICE_NAME, SERVICE_ALL_ACCESS);
  if(!service) {
    if(!install) {
      WinError("unable to open service", GetLastError());
      CloseServiceHandle(SCman);
      return;
    }
    service = CreateService(SCman, SERVICE_NAME, "Yoctopuce Sensor server",
                            SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS,
                            SERVICE_AUTO_START, SERVICE_ERROR_NORMAL,
                            path, NULL, NULL, NULL, NULL, NULL);
    if (!service) {
      WinError("CreateService failed", GetLastError());
      CloseServiceHandle(SCman);
      return;
    }
    fprintf(stderr, "Service successfully installed\n");
  } else {
    if(!install) {
      SERVICE_STATUS dummy;
      ControlService(service, SERVICE_CONTROL_STOP, &dummy);
      if(DeleteService(service)) {
        fprintf(stderr, "Service successfully uninstalled\n");
      }
      return;
    }
    fprintf(stderr, "Service already installed\n");
  }
  StartService(service, 0, 0);
  CloseServiceHandle(service);
  CloseServiceHandle(SCman);
}

void ReportServiceStatus( u32 CurrentState, u32 Win32ExitCode, u32 WaitHint)
{
  static DWORD dwCheckPoint = 1;

  ServiceStatus.dwCurrentState = CurrentState;
  ServiceStatus.dwWin32ExitCode = Win32ExitCode;
  ServiceStatus.dwWaitHint = WaitHint;

  if (CurrentState == SERVICE_START_PENDING)
    ServiceStatus.dwControlsAccepted = 0;
  else
    ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;

  if ( (CurrentState == SERVICE_RUNNING) || (CurrentState == SERVICE_STOPPED) )
    ServiceStatus.dwCheckPoint = 0;
  else
    ServiceStatus.dwCheckPoint = dwCheckPoint++;

  SetServiceStatus(ServiceStatusHandle, &ServiceStatus);
}

//   Handle service control functions
//
void  ServiceControlhandler(DWORD ctrl, DWORD eventType, LPVOID eventData, LPVOID contex)
{
  switch(ctrl) {
  case SERVICE_CONTROL_STOP:
    ReportServiceStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
    Globalp.runMore = 0;
    break;
  default:
    break;
  }
  ReportServiceStatus(ServiceStatus.dwCurrentState, NO_ERROR, 0);
}

int winmain(int argc, char *argv[])
{
  WSADATA wsaData;
  int     iResult;

  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    WinError("Unable to start Winsock2", iResult);
    return -1;
  }
  return ServerMain();
}

void WINAPI SvcMain(DWORD dwArgc, LPTSTR *lpszArgv)
{
  ServiceStatusHandle = RegisterServiceCtrlHandlerEx(SERVICE_NAME,
                        (LPHANDLER_FUNCTION_EX)ServiceControlhandler, 0);
  if(!ServiceStatusHandle) {
    WinError("Unable to register Service handler", GetLastError());
    return;
  }
  ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  ServiceStatus.dwServiceSpecificExitCode = 0;
  ReportServiceStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
  ReportServiceStatus(SERVICE_RUNNING, NO_ERROR, 0);
  winmain(dwArgc, lpszArgv);
  ReportServiceStatus(SERVICE_STOPPED, NO_ERROR, 0);
  return;
}

int main (int argc, char *argv[])
{
  SERVICE_TABLE_ENTRY DispatchTable[] = {
    { SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)SvcMain },
    { NULL, NULL }
  };
  ParseArguments(argc, argv);
  if (Globalp.runAsDaemon) {
    // Service Mode
    if (!StartServiceCtrlDispatcher(DispatchTable)) {
      u32 error = GetLastError();
      WinError("Unable to Start the service", error);
      return 1;
    }
  } else {
    return winmain(argc, argv);
  }
}

#else
// Unix version


int main (int argc, char *argv[])
{
  signal(SIGPIPE, SIG_IGN);

  ParseArguments(argc, argv);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
  if (Globalp.runAsDaemon) {
    if (daemon(0, 0) < 0) {
      exit(1);
    }
  }
#pragma clang diagnostic pop

  return ServerMain();
}

#endif

////////////////////////////////////////////////////////////////////////////////
//
//               Some helper functions for handling TCP sockets
//
////////////////////////////////////////////////////////////////////////////////

typedef struct _TCPclient {
  SOCKET  sock;
  struct _TCPclient *next;
} TCPclient;

TCPclient *clients = NULL;

// Create a TCP socket listening for connections
//
static SOCKET newTCPListener(unsigned portNo)
{
  int     optval;
  struct  sockaddr_in srvaddr;
  SOCKET  srvsock;

  srvsock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (srvsock == INVALID_SOCKET) {
    return INVALID_SOCKET;
  }
  optval = 1;
  setsockopt(srvsock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port   = htons(portNo);
  srvaddr.sin_addr.s_addr = INADDR_ANY;
  if (bind(srvsock, (struct sockaddr *)&srvaddr, sizeof(srvaddr)) < 0) {
    return INVALID_SOCKET;
  }
  if (listen(srvsock, 5) < 0) {
    return INVALID_SOCKET;
  }
  return srvsock;
}

// Accept any incoming connection on given socket
//
static void TCPAccept(SOCKET srvsock)
{
#ifdef SO_NOSIGPIPE
  int         noSigpipe = 1;
#endif
  fd_set      fds;
  socklen_t	addrlen;
  SOCKET      sock;
  TCPclient   *newClient;

  struct sockaddr_in remote;
  struct timeval     timeout;
  memset(&timeout, 0, sizeof(timeout));
  timeout.tv_sec = 0;
  timeout.tv_usec = 1000;
  FD_ZERO(&fds);
  FD_SET(srvsock, &fds);
  if(select((int)srvsock + 1, &fds, NULL, NULL, &timeout) <= 0) {
    return;
  }
  if(FD_ISSET(srvsock, &fds)) {
    addrlen = sizeof(remote);
    sock = accept(srvsock, (struct sockaddr *)&remote, &addrlen);
    if (sock == INVALID_SOCKET) {
      return;
    }
#ifdef SO_NOSIGPIPE
    setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (void *)&noSigpipe, sizeof(int));
#endif
    newClient = (TCPclient *)malloc(sizeof(TCPclient));
    newClient->sock = sock;
    newClient->next = clients;
    clients = newClient;
  }
}

// Send a string to all clients
//
static void TCPBroadcast(const char *str)
{
  TCPclient   *client = clients;
  TCPclient   **prev = &clients;

  while(client) {
    // send line to first client in list
    if(send(client->sock, str, strlen(str), 0) < 0) {
      // send error: remove disconnected client
      closesocket(client->sock);
      *prev = client->next;
      free(client);
    } else {
      // success, move on to next client
      prev = &(client->next);
    }
    client = *prev;
  }
}

////////////////////////////////////////////////////////////////////////////////
//
//                   And now the real USB sensor server code
//
////////////////////////////////////////////////////////////////////////////////

using namespace std;

static void sensorTimedReportCallBack(YSensor *fct, YMeasure measure)
{
  const char *header = (Globalp.encodeNMEA ? "$YS" : "");
  char       buf[512];

  snprintf(buf, sizeof(buf), "%s%s,%.3f,%s", header,
           fct->get_logicalName().c_str(), measure.get_averageValue(), fct->get_unit().c_str());

  if(Globalp.encodeNMEA) {
    // NMEA format: compute checksum and append it to the message
    int      i, len = (int)strlen(buf);
    unsigned chksum = 0;
    for(i = 1; i < len; i++) {
      chksum ^= (unsigned char)buf[i];
    }
    snprintf(buf + len, sizeof(buf) - len, "*%02X\r\n", chksum);
  } else {
    // raw format, just append CR-LF
    strcat(buf, "\r\n");
  }
  TCPBroadcast(buf);
}

static void deviceArrival(YModule *m)
{
  string serial = m->get_serialNumber();
  int    count = 0;

  cout << "Device arrival : " << serial << endl;

  YSensor *sensor = YSensor::FirstSensor();
  while(sensor) {
    string name = sensor->get_logicalName();
    if(name.length() > 0 && sensor->get_module()->get_serialNumber() == serial) {
      cout << "- " << name << endl;
      sensor->set_reportFrequency(Globalp.freq);
      sensor->registerTimedReportCallback(sensorTimedReportCallBack);
      count++;
    }
    sensor = sensor->nextSensor();
  }

  if(!count) {
    cout << "(no logical name given to any functions, ignoring sensor)" << endl;
  }

}

static void deviceRemoval(YModule *m)
{
  cout << "Device removal : " << m->get_serialNumber() << endl;
}

static void log(const string& val)
{
  cout << val;
}

int ServerMain(void)
{
  string  errmsg;
  SOCKET  srvsock;

  // Detect devices connected on USB (handle hot-plug)
  YAPI::RegisterLogFunction(log);
  YAPI::RegisterDeviceArrivalCallback(deviceArrival);
  YAPI::RegisterDeviceRemovalCallback(deviceRemoval);
  YAPI::DisableExceptions();
  if (YAPI::RegisterHub("usb", errmsg) != YAPI::SUCCESS) {
    cerr << "RegisterHub error : " << errmsg << endl;
    return 1;
  }

  // Create TCP/IP server socket
  srvsock = newTCPListener(Globalp.portno);
  if(srvsock == INVALID_SOCKET) {
    cerr << "Cannot listen on TCP port " << Globalp.portno << endl;
    return 1;
  }
  cerr << "Listening on TCP port " << Globalp.portno << endl;

  // Run indefinitely
  while (Globalp.runMore) {
    TCPAccept(srvsock);
    YAPI::UpdateDeviceList(errmsg); // traps plug/unplug events
    YAPI::Sleep(300, errmsg);       // traps others events
  }
  return 0;
}
