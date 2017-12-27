#include"afxmt.h"//使用cs
#include<fstream>
#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
using namespace std;
#pragma comment (lib,"Ws2_32.lib")
#define default_port "5000"
#define default_buffer_len 1024
#define max_thread 5
CRITICAL_SECTION cs;
struct threadinfo
{
	SOCKET connect_client;
	long long count_size;
	int thread_num;
	long long seek;
};
fstream FileCopy("e:/ubuntukylinCopy.iso", ios::out | ios::binary);// e:/clientCopy.txt
DWORD WINAPI ThreadProcess(LPVOID lpParameter)
{
	cout << "接收线程ID：" << GetCurrentThreadId() << endl;
	threadinfo *info = (threadinfo *)lpParameter;
	FileCopy.seekp(info->seek, ios::beg);

	//cout << GetCurrentThreadId() << " 文件开始写入的seek：" << FileCopy.tellp() << endl;

	long long num = 0;
	char temp[default_buffer_len];
	long long len = info->count_size;

	//cout << GetCurrentThreadId() << " 本次写入的长度：" << len << endl;

	while(true)
	{
		//cout << GetCurrentThreadId() << " 开始写入前的seek：" << FileCopy.tellg() << endl;
		memset(temp, 0, sizeof(temp));
		if (len < default_buffer_len)
		{
			num = recv(info->connect_client, temp, len, 0);
			FileCopy.write(temp, len);
			//cout << GetCurrentThreadId() << " " << num << " " << len << endl;
			break;
		}
		num = recv(info->connect_client, temp, default_buffer_len, 0);
		FileCopy.write(temp, default_buffer_len);
		//cout << temp;
		len -= num;
		//cout << GetCurrentThreadId() << " " << num << " " << len << endl;
		//cout << GetCurrentThreadId() <<" "<< FileCopy.tellp() << endl; 
	}
	//cout << GetCurrentThreadId() << " 写入完成后文件长度：" << FileCopy.tellp() << endl;
	cout << "接收线程ID" << GetCurrentThreadId() << "完成..." << endl;
	return 0;
}
int main()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	ADDRINFO *result, hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	getaddrinfo("127.0.0.1", default_port, &hints, &result);
	SOCKET connect_client = INVALID_SOCKET;
	connect_client = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	connect(connect_client, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);
	cout << "已经连接..." << endl;
	long long file_len;
	recv(connect_client, (char *)& file_len, sizeof(file_len), 0);
	cout << "文件长度：" << file_len << endl;
	long long count_size = file_len / max_thread;
	HANDLE thread[max_thread];
	InitializeCriticalSection(&cs);
	int i;
	threadinfo info[max_thread];
	memset(info, 0, sizeof(info));
	for (i = 0; i < max_thread; i++)
	{
		info[i].connect_client = connect_client;
		info[i].count_size = count_size;
		info[i].thread_num = i;
		info[i].seek = i*count_size;
	}
	info[max_thread - 1].count_size = file_len - (max_thread - 1) * count_size;
	for (i = 0; i < max_thread; i++)
	{
		EnterCriticalSection(&cs);
		thread[i] = CreateThread(NULL, 0, ThreadProcess, &info[i], 0, NULL);
		Sleep(500);
		LeaveCriticalSection(&cs);
	}
	WaitForMultipleObjects(max_thread, thread, true, INFINITE);
	DeleteCriticalSection(&cs);
	for (i = 0; i < max_thread; i++)
	{
		CloseHandle(thread[i]);
	}
	closesocket(connect_client);
	system("pause");
	return 0;
}