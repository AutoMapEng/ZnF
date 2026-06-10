#pragma once


typedef struct _TASK_LIST
{
  DWORD   dwProcessId;
  char    ProcessName[MAX_PATH];
} TASK_LIST, *PTASK_LIST;

PTASK_LIST GetTaskList( LPLONG pNumTasks );


#include <string>
#include <vector>
using namespace std;

class ProcessInfo
{
public:
  string name; // the filename of the process.
  int id32; // >0 if it's a 32-bit process.
  int id16; // >0 if it's a 16-bit process.
};

// an alternate way to get a list of running processes.
BOOL EnumProcs(vector<ProcessInfo> &rProcessList);

