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
	fstream file("e:/大物.rar", ios::in | ios::binary);
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;
	getaddrinfo(NULL, default_port, &hints, &result);
	SOCKET listen_socket = INVALID_SOCKET;
	listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(listen_socket, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);
	listen(listen_socket, SOMAXCONN);
	SOCKET client_socket = INVALID_SOCKET;
	client_socket = accept(listen_socket, NULL, NULL);
	closesocket(listen_socket);
	cout << "已经连接..." << endl;
	while (true)
	{
		if (file.eof())
			break;
		file.read(temp, default_buffer_len);
		send(client_socket, temp, default_buffer_len, 0);
		//cout << temp;
	}
	file.close();
	shutdown(client_socket, SD_SEND);
	closesocket(client_socket);
	WSACleanup();
	system("pause");
    return 0;
}