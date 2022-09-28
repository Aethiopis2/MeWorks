//-----------------------------------------------------------------------------------|
// Sending_TCP_Client -- the following console application creates a client service
//	and sends a stream of text messages to the server process listeining on the 
//	network.
//
// By: Code Red
//
// Date created:	11th of January 2016, Monday
// Last update:		11th of January 2016, Monday
//-----------------------------------------------------------------------------------|
// Mr. VS please include in the winsocks library
//	during your linking phase
#pragma comment (lib, "ws2_32.lib")

//-----------------------------------------------------------------------------------|
// DEFINES																			 |
//-----------------------------------------------------------------------------------|
#define WIN32_LEAN_AND_MEAN

//-----------------------------------------------------------------------------------|
// INCLUDES																			 |
//-----------------------------------------------------------------------------------|
#include <stdio.h>
#include <conio.h>

#include <WinSock2.h>

// Enter main
int main()
{
	// main local's first
	WSADATA			wsa_data;		// used by the startup function
	SOCKET			client_socket;	// the client socket used for communication
	SOCKADDR_IN		server_addr;	// the server address to be connected with
	int				port = 80;	// the port to commune the server with
	int				bytes_sent;		// the bytes sent

	char buffer[] = "Message from a young hacker: \"Hello Networked World!!!\"";

	// load the winsock dll 2.2
	if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
	{
		// error
		printf("Client: \"WSAStartup(...)\" failed! Error code -- %ld\n",
			WSAGetLastError());

		// hold up console
		printf("\n\nPress any key to exit...\n");
		getch();

		return -1;
	} // end if startup failure

	// print messages
	printf("Client: The Winsock DLL 2.2 has been loaded.\n");
	printf("Client: The current DLL status is -- %s\n", wsa_data.szSystemStatus);

	// set the socket
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// test for a valid socket
	if (client_socket == INVALID_SOCKET)
	{
		// error
		printf("Clent: \"socket(...)\" failed! Error code -- %ld\n",
			WSAGetLastError());

		// do clean up
		WSACleanup();

		// hold up console
		printf("\n\nPress any key to exit...\n");
		getch();

		return -1;
	} // end if invalid socket

	// print socket ok message
	printf("Client: \"socket(...)\" is OK!\n");

	// fill in the server address to connect with
	server_addr.sin_addr.s_addr = inet_addr("192.168.1.11");
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);

	// make the connection with the server
	if (connect(client_socket, (SOCKADDR *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		// error
		printf("Clent: \"connect(...)\" failed! Error code -- %ld\n",
			WSAGetLastError());

		// close socket
		closesocket(client_socket);

		// do clean up
		WSACleanup();

		// hold up console
		printf("\n\nPress any key to exit...\n");
		getch();

		return -1;
	} // end if socket error

	// ok message
	printf("Client: \"connect(...)\" is OK!\nClient: sending text to server...\n");
	//getch();
	bytes_sent = send(client_socket, buffer, sizeof(buffer), 0);

	// now send the stream of characters to the server
	if (bytes_sent == 0) 
	{
		// error
		printf("Clent: \"send(...)\" failed! Error code -- %ld\n",
			WSAGetLastError());

		// close socket
		closesocket(client_socket);

		// do clean up
		WSACleanup();

		// hold up console
		printf("\n\nPress any key to exit...\n");
		getch();

		return -1;
	} // end if

	// ok messages
	printf("Client: Successuly sent %d bytes of data to server.\n", bytes_sent);

	// close the socket
	closesocket(client_socket);

	// do clean up
	WSACleanup();

	// hold up console
	printf("\n\nPress any key to exit...\n");
	getch();

	return 0;

} // end main