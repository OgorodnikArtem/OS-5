#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string.h>
#include <vector>
#include <algorithm>

using namespace std;

struct Array
{
	int n;
	char arr[100];
};

int main(int argc, char* argv[])
{
	HANDLE hReadChannelReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LPCSTR)"WRITE_CHANNEL_READY");
	HANDLE hWriteChannelReady = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LPCSTR)"READ_CHANNEL_READY");
	//HANDLE hWriteChannelHasBeenRead = OpenEvent(EVENT_ALL_ACCESS, FALSE, (LPCWSTR)"READ_CHANNEL_HAS_BEEN_READ");

	HANDLE hWritePipe = (HANDLE)atoi(argv[1]), hReadPipe = (HANDLE)atoi(argv[0]);
	int dwBytesWritten = 0;
	Array arr;

	WaitForSingleObject(hReadChannelReady, INFINITE);

	if (!ReadFile(hReadPipe, (char*)&arr, sizeof(Array), (LPDWORD)&dwBytesWritten, NULL))
	{
		_cputs("Read from file failed.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return GetLastError();
	}

	Array result;
	result.n = arr.n;

	std::cout << "Original array:\nSize: " << arr.n << endl;
	std::cout << "New array:\nSize: " << result.n << endl;

	vector<int> v;
	for (int i = 0; i < arr.n; ++i)
	{
		v.push_back(arr.arr[i]);
		cout << "i:" <<  static_cast<int>(v[i]) << " ";

	}

	cout << endl;

	sort(v.begin(), v.end());

	for (int i = 0; i < arr.n; ++i)
	{
		v.push_back(arr.arr[i]);
		cout << "s_i :" << static_cast<int>(v[i]) << " ";

	}

	cout << endl;

	for (int i = 0; i < arr.n; ++i)
	{
		result.arr[i] = v[i];

	}

	if (!WriteFile(hWritePipe, &(result.n), sizeof(int), (LPDWORD)&dwBytesWritten, NULL))
	{
		_cputs("Write to file failed.\n");
		_cputs("Press any key to finish.\n");
		_getch();
		return GetLastError();
	}

	SetEvent(hWriteChannelReady);

	for (int i = 0; i <= result.n; ++i)
	{
		WaitForSingleObject(hReadChannelReady, INFINITE);
		if (!WriteFile(hWritePipe, &(result.arr[i]), sizeof(result.arr[i]), (LPDWORD)&dwBytesWritten, NULL))
		{
			_cputs("Write to file failed.\n");
			_cputs("Press any key to finish.\n");
			_getch();
			return GetLastError();
		}
		SetEvent(hWriteChannelReady);

	}

	_cputs("Press any key to finish.\n");
	_getch();
	return 0;
}