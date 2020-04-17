#include <iostream>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

void main()
{
	// Initialize winsock
	WSADATA wsa_data;
	const auto version = MAKEWORD(2, 2);

	const auto ws_ok = WSAStartup(version, &wsa_data);
	if (ws_ok != 0)
	{
		std::cerr << "Unable to initialize winsock! Exiting..." << std::endl;
		return;
	}
	// Create Socket
	const auto listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listening_socket == INVALID_SOCKET)
	{
		std::cerr << "Can't Create the socket! Exiting..." << std::endl;
		return;
	}
	// Bind address and port
	sockaddr_in address{};
	address.sin_family = AF_INET;
	address.sin_port = htons(54000);
	address.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
	// Configure winsock for listening_socket
	listen(listening_socket, SOMAXCONN);

	// Wait for Connection
	sockaddr_in client{};
	int client_size = sizeof(client);
	const auto client_socket = accept(listening_socket, reinterpret_cast<sockaddr*>(&client), &client_size);

	char host[NI_MAXHOST]; // Client's remote name
	char service[NI_MAXSERV]; // Port

	memset(host, 0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	const auto connected = getnameinfo(reinterpret_cast<sockaddr*>(&client), client_size, host, NI_MAXHOST, service, NI_MAXSERV, 0);
	if (connected == 0)
	{
		std::cout << "Connected on Port: " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);

		std::cout << "Connected on Port: " << ntohs(client.sin_port) << std::endl;
	}

	// Close socket
	closesocket(listening_socket);

	// Accept and Echo back message
	const auto max_buffer = 4096;
	char buffer[max_buffer]; // Should be until no bytes

	while(true)
	{
		memset(buffer, 0, max_buffer);

		const auto bytes_received = recv(client_socket, buffer, max_buffer, 0);
		if (bytes_received == SOCKET_ERROR || bytes_received == 0)
		{
			std::cerr << "Error Receiving Message! Exiting..." << std::endl;
			break;
		}

		// Echo back to Client
		send(client_socket, buffer, bytes_received + 1, 0); // Don't know about +1
	}

	// Close client socket
	closesocket(client_socket);

	// Cleanup winsock
	WSACleanup();
}