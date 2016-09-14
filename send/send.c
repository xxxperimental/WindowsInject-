#include <windows.h>
#include <TlHelp32.h>
char* getProcList();

BOOL sendData();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char* result  = getProcList();

	if(sendData())
	{
		MessageBox(NULL, "Send - OK", "Info", MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(NULL, "Unable send data", "Error", MB_ICONERROR);
	}
	return 0;
}

char* getProcList()
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

BOOL sendData()
{
	
}