#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

// Skip initial argument in a command line.  Adapted from my winserv
// http://www.sw4me.com/wiki/Winserv
static LPTSTR skip_arg(LPTSTR p)
{
  int inquote, slashes;

  // skip spaces
  while (isspace(*p)) { p++; }
  if (*p == TEXT('\0')) { return p; }
  inquote = 0; slashes = 0;
  // we found where the argument begins
  for (;;) {
    while (*p == TEXT('\\')) {
      slashes++;
      p++;
    }
    if (*p == TEXT('"')) {
      if ((slashes & 1) == 0) {
	if ((inquote) && (p[1] == TEXT('"'))) {
	  p++;
	} else {
	  inquote = !inquote;
	}
      }
    }
    slashes=0;
    if ((*p == TEXT('\0')) || (!inquote && isspace(*p))) {
      while (isspace(*p))
	p++;
      return p;
    }
    p++;
  }
}

// Terminate unsuccessfully with an error message
static void Fail(const char *desc) {
  fprintf(stderr, "Error %u in %s\n", GetLastError(), desc);
  exit(1);
}

// Raymond Chen's CreateProcessInJob
// see https://devblogs.microsoft.com/oldnewthing/20131209-00/?p=2433
BOOL CreateProcessInJob(
			HANDLE hJob,
			LPCTSTR lpApplicationName,
			LPTSTR lpCommandLine,
			LPSECURITY_ATTRIBUTES lpProcessAttributes,
			LPSECURITY_ATTRIBUTES lpThreadAttributes,
			BOOL bInheritHandles,
			DWORD dwCreationFlags,
			LPVOID lpEnvironment,
			LPCTSTR lpCurrentDirectory,
			LPSTARTUPINFO lpStartupInfo,
			LPPROCESS_INFORMATION ppi)
{
  BOOL fRc = CreateProcess(
			   lpApplicationName,
			   lpCommandLine,
			   lpProcessAttributes,
			   lpThreadAttributes,
			   bInheritHandles,
			   dwCreationFlags | CREATE_SUSPENDED,
			   lpEnvironment,
			   lpCurrentDirectory,
			   lpStartupInfo,
			   ppi);
  if (fRc) {
    fRc = AssignProcessToJobObject(hJob, ppi->hProcess);
    if (fRc && !(dwCreationFlags & CREATE_SUSPENDED)) {
      fRc = ResumeThread(ppi->hThread) != (DWORD)-1;
    }
    if (!fRc) {
      TerminateProcess(ppi->hProcess, 0);
      CloseHandle(ppi->hProcess);
      CloseHandle(ppi->hThread);
      ppi->hProcess = ppi->hThread = NULL;
    }
  }
  return fRc;
}

// defined in pijhelp.cc, so I can use C++ raw literals
extern const char *Help;


int main(int argc, char*argv[])
{
  if (argc <= 1) {
    printf("%s",Help);
    exit(0);
  }
  LPTSTR cmdline = GetCommandLineW();
  LPTSTR restCmdLine = skip_arg(cmdline);
  HANDLE hJob = CreateJobObject(NULL,NULL);
  if (hJob == NULL) {
    Fail("CreateJobObject");
  }
  HANDLE hPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 1);
  if (hPort == NULL) {
    Fail("CreateIoCompletionPort");
  }

  // see https://devblogs.microsoft.com/oldnewthing/20130405-00/?p=4743
  JOBOBJECT_ASSOCIATE_COMPLETION_PORT jacp;
  jacp.CompletionKey = hJob;
  jacp.CompletionPort = hPort;
  if (!SetInformationJobObject(hJob,
			       JobObjectAssociateCompletionPortInformation,
			       &jacp, sizeof(jacp))) {
    Fail("SetInformationJobObject(JobObjectAssociateCompletionPortInformation)");
  }

  STARTUPINFO si = { sizeof si };
  PROCESS_INFORMATION pi;
  if (!CreateProcessInJob(hJob,
			  NULL,
			  restCmdLine,
			  NULL,
			  NULL,
			  FALSE,
			  0,
			  NULL,
			  NULL,
			  &si,
			  &pi)) {
    Fail("CreateProcess");
  }
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);


  DWORD completionCode;
  ULONG_PTR completionKey;
  LPOVERLAPPED overlapped;

  for (;;) {
    BOOL rc = GetQueuedCompletionStatus(hPort,
					&completionCode,
					&completionKey,
					&overlapped,
					INFINITE);
    if (!rc) {
      Fail("GetQueuedCompletionStatus");
    }
    if ((HANDLE)completionKey == hJob &&
	completionCode == JOB_OBJECT_MSG_ACTIVE_PROCESS_ZERO) {
      break;
    }
  }
  CloseHandle(hJob);
  return 0;
}
