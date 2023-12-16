#include <iostream>
#include <windows.h>
#include <conio.h>
#include <time.h>

struct Array
{
	int n;
	char arr[100];
};

int main()
{
	HANDLE hWriteChannelReady = CreateEvent(NULL, FALSE, FALSE, (LPCSTR)"WRITE_CHANNEL_READY");
	HANDLE hReadChannelReady = CreateEvent(NULL, FALSE, FALSE, (LPCSTR)"READ_CHANNEL_READY");



	srand(time(NULL));



	Array arr;
	std::cout << "Enter array size: ";
	std::cin >> arr.n;
	
	for (int i = 0; i < arr.n; i++) {
		arr.arr[i] = rand() % 256 - 128;
	}

	for (int i = 0; i < arr.n; i++) {
		std::cout << static_cast<int>(arr.arr[i]) << " ";
	}

	std::cout << std::endl;

	HANDLE hWritePipe, hReadPipe;
	HANDLE hWritePipe2, hReadPipe2;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0))
	{
		_cputs("Create pipe failed.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return GetLastError();
	}

	if (!CreatePipe(&hReadPipe2, &hWritePipe2, &sa, 0))
	{
		_cputs("Create pipe failed.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return GetLastError();
	}

	char lpszAppName[] = "Sort.exe";
	char lpszParameter[100];
	sprintf_s(lpszParameter, "%d %d", (int)hReadPipe, (int)hWritePipe2);

	STARTUPINFO si;
	PROCESS_INFORMATION piApp;

	ZeroMemory(&si, sizeof(STARTUPINFO));
	if (!CreateProcessA((LPCSTR)lpszAppName, lpszParameter, nullptr, nullptr, TRUE,
		CREATE_NEW_CONSOLE, nullptr, nullptr, (LPSTARTUPINFOA)&si, &piApp))
	{
		_cputs("The new process is not created.\n");
		_cputs("Check a name of the process.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return 0;
	}

	int dwBytesWritten = 0;
	if (!WriteFile(hWritePipe, (char*)&arr, sizeof(Array), (LPDWORD)&dwBytesWritten, NULL))
	{
		_cputs("Write to file failed.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return GetLastError();
	}

	SetEvent(hWriteChannelReady);

	WaitForSingleObject(hReadChannelReady, INFINITE);

	Array result;
	if (!ReadFile(hReadPipe2, &(result.n), sizeof(int), (LPDWORD)&dwBytesWritten, NULL))
	{
		_cputs("Read from file failed.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return GetLastError();
	}

	SetEvent(hWriteChannelReady);

	for (int i = 0; i < result.n; ++i)
	{
		WaitForSingleObject(hReadChannelReady, INFINITE);
		if (!ReadFile(hReadPipe2, &(result.arr[i]), sizeof(result.arr[i]), (LPDWORD)&dwBytesWritten, NULL))
		{
			_cputs("Read from file failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return GetLastError();
		}
		SetEvent(hWriteChannelReady);
	}

	std::cout << "Array sort : ";
	for (int i = 0; i < result.n; ++i)
	{

		std::cout << static_cast<int>(result.arr[i]) << " ";
	}

	CloseHandle(hWriteChannelReady);
	CloseHandle(hReadChannelReady);
	CloseHandle(piApp.hThread);
	CloseHandle(piApp.hProcess);
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(hWritePipe2);
	CloseHandle(hReadPipe2);
	return 0;
}