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
fstream file("d:/迅雷下载/安装包/ubuntukylin.iso", ios::in | ios::binary);//   e:/client.txt
struct  threadinfo
{
	SOCKET client_socket;
	long long count_size;
	int thread_num;
	long long seek;
};
DWORD WINAPI ThreadProcess(LPVOID lpParameter)
{
	cout << "发送线程ID：" << GetCurrentThreadId() << endl;
	threadinfo *info = (threadinfo *)lpParameter;
	long long num = 0;
	char temp[default_buffer_len];
	file.seekg(info->seek, ios::beg);
	long long len = info->count_size;
	while(true)
	{
		memset(temp, 0, sizeof(temp));
		file.read(temp, default_buffer_len);
		if (len < default_buffer_len)
		{
			num = send(info->client_socket, temp, len, 0);
			break;
		}
		num = send(info->client_socket, temp, default_buffer_len, 0);
		//cout << temp;
		len -= num;
		//cout << GetCurrentThreadId() << " " << num << " " << len << endl;
	}
	cout << "发送线程ID" << GetCurrentThreadId() << "完成..." << endl;
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
	getaddrinfo(NULL, default_port, &hints, &result);
	SOCKET listen_socket = INVALID_SOCKET;
	listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	::bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);
	listen(listen_socket, SOMAXCONN);
	SOCKET client_socket = INVALID_SOCKET;
	client_socket = accept(listen_socket, NULL, NULL);
	closesocket(listen_socket);
	cout << "已经连接..." << endl;
	file.seekg(0, ios::end);
	long long file_len = file.tellg();
	file.seekg(0, ios::beg);
	send(client_socket, (char *)&file_len, sizeof(file_len), 0);
	cout << "文件长度：" << file_len << endl;
	long long count_size = file_len / max_thread;
	HANDLE thread[max_thread];
	InitializeCriticalSection(&cs);
	int i;
	threadinfo info[max_thread];
	memset(info, 0, sizeof(info));
	for (i = 0; i < max_thread; i++)
	{
		info[i].client_socket = client_socket;
		info[i].count_size = count_size;
		info[i].thread_num = i;
		info[i].seek = i*count_size;
	}
	info[max_thread-1].count_size = file_len - (max_thread-1) * count_size;
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
	file.close();
	shutdown(client_socket, SD_SEND);
	closesocket(client_socket);
	WSACleanup();
	system("pause");
	return 0;
}