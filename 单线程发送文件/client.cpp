//#pragma warning(disable:4996)
#include<iostream>
#include<WinSock2.h>
#include<fstream>
#include<WS2tcpip.h> 
using namespace std;
#pragma comment(lib,"Ws2_32.lib")
#define default_port "5000"
#define default_buffer_len 5000
int main()
{
	WSADATA wsadata;
	ADDRINFO *result, hints;
	char temp[default_buffer_len];
	WSAStartup(MAKEWORD(2, 2), &wsadata);
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
	int num = 0;
	fstream file("e:/大物copy.rar", ios::out|ios::binary);
	while (true)
	{
		num = recv(connect_client, temp, default_buffer_len, 0);
		if (num == 0)
			break;
		//cout << temp;
		file.write(temp, num);
	}
	file.close();
	shutdown(connect_client, SD_SEND);
	closesocket(connect_client);
	WSACleanup();
	system("pause");
    return 0;
}