//------------------------------------------------------------------|
// Receving_TCP_Server - A console application that implements full
//	fledged server side data receiver on a LAN/Wifi network. The 
//	server basically prints whatever it received from the client 
//	process or various error codes depending on what happend.
//
// NOTE: This server process does not send messages to clients besides
//	the messages sent during the TCP/IP protocol.
//
// By: Code Red
//
// Date created:	January 10th 2016, Sunday
// Last update:		January 11th 2016, Monday
//------------------------------------------------------------------|

// Mr Visual Studio please inject the WinSockets library during 
//	linking into my code
#pragma comment(lib, "WS2_32.LIB")

//------------------------------------------------------------------|
// DEFINES															|
//------------------------------------------------------------------|
#define WIN32_LEAN_AND_MEAN					// just say no to MFC

//------------------------------------------------------------------|
// INCLUDES															|
//------------------------------------------------------------------|
#include <stdio.h>
#include <conio.h>

#include <WinSock2.h>

//------------------------------------------------------------------|
// FUNCTIONS														|
//------------------------------------------------------------------|

// Enter main like Bruce Lee from Enter the Dragon
int main()
{
	// main locals
	WSADATA				wsa_data;			// this is used by the startup function
	SOCKET				listening_socket;	// used to listen incomming connections on the
											//	server interfacce
	SOCKET				new_connection;		// used to temporarliy hold socket info during
											//	calls to accept
	SOCKADDR_IN			server_addr;		// stores the inteface and potocol info of the 
											//	server
	SOCKADDR_IN			client_info;		// stores the interface info of the client
	int					port = 80;			// the listening port used by the server
	char				buffer[1024];		// buffer used to store sent messages by the client
	int					bytes_recvd;		// the number of bytes received from clients

	// load the winsock dll 2.2
	if (WSAStartup(MAKEWORD(2,2), &wsa_data) != 0)
	{
		// error
		printf("Server: \"WSAStartup(...)\" falied! Error code %ld\n",
			WSAGetLastError());

		// hold console for me before closing
		printf("\n\nPress any key to exit...\n");
		_getch();

		// back to windows
		return(-1);
	} // end if WSAStartup

	// print some messages
	printf("Server: Loaded Winsock DLL 2.2\nServer: DLL status is %s\n", 
		wsa_data.szSystemStatus);

	// set the socket to be used as listener
	listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// test for an invalid socket
	if (listening_socket == INVALID_SOCKET)
	{
		// error
		printf("Server: \"socket(...)\" falied! Error code %ld\n",
			WSAGetLastError());

		// unload the dll
		WSACleanup();

		// hold console for me before closing
		printf("\n\nPress any key to exit...\n");
		_getch();

		// back to windows
		return(-1);
	} // end if socket error

	// print OK message
	printf("Server: \"socket(...)\" is OK!\n");

	// next fill-in the server address structure, which is
	//	necessary for calls to bind()
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// listen on all interfaces
	server_addr.sin_family = AF_INET;					// IPv4
	server_addr.sin_port = htons(port);					// the listening port

	// now make the call to bind() to bind the socket to this addr
	if (bind(listening_socket, (SOCKADDR *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		// error
		printf("Server: \"bind(...)\" falied! Error code %ld\n",
			WSAGetLastError());

		// close the socket
		closesocket(listening_socket);

		// unload dll
		WSACleanup();

		// hold console for me before closing
		printf("\n\nPress any key to exit...\n");
		_getch();

		// back to windows
		return(-1);
	} // end if bind failed

	// print message of fine
	printf("Server: \"bind(...)\" is OK!\n");

	// at this point our dear little server can listen to incomming connections
	//	start listening with a backlog of 5 connections at once
	if (listen(listening_socket, 5) == SOCKET_ERROR)
	{
		// error
		printf("Server: \"bind(...)\" falied! Error code %ld\n",
			WSAGetLastError());

		// close the socket
		closesocket(listening_socket);

		// unload dll
		WSACleanup();

		// hold console for me before closing
		printf("\n\nPress any key to exit...\n");
		_getch();

		// back to windows
		return(-1);
	} // end if listen failed

	// listen is up and running print the positive replies
	printf("Server: \"listen(...)\" is OK!\n");
	printf("Server: Up and running, I'm waiting for connections Doc...\n");

	// sit in a tight loop and accept the very first connection in your vicinity
	new_connection = SOCKET_ERROR;				// assume is empty at first
	while (new_connection == SOCKET_ERROR)
	{
		// accept any incoming connections
		new_connection = accept(listening_socket, NULL, NULL);

		// print messages
		printf("Server: \"accept(...)\" is OK!\n");

		// test if new_connection is valid
		if (new_connection != SOCKET_ERROR)
		{
			// print
			printf("Server: Client got connected!\n");
			printf("Server: Printing client info...\n");

			// clear the client info structure
			memset(&client_info, 0, sizeof(client_info));

			int siz = sizeof(client_info);

			// acquire the client information
			getpeername(new_connection, (SOCKADDR *)&client_info, &siz);

			// print this info
			printf("Server: Client IP -- %s\n", inet_ntoa(client_info.sin_addr));
			printf("Server: Ready to recieve client data.\n");

			// receive the data transmission from client
			bytes_recvd = recv(new_connection, buffer, sizeof(buffer), 0);

			// test if there is something actually recieved
			if (bytes_recvd > 0)
			{
				// print messages
				printf("Server: \"recv(...)\" is OK!\n");
				printf("Server: Recived %d bytes from client.\n", bytes_recvd);
				printf("Server: Printing what client sent onto the console...\n\t");

				// loop and print whatever client sent as ASCII encoded characters
				for (size_t i = 0; i < bytes_recvd; i++)
					printf("%c", buffer[i]);
				printf("\n");
			} // end if recvd
			else if (bytes_recvd == 0)
			{
				printf("Server: connection closed!\n");
			} // end else if
			else
			{
				printf("Server: \"recv(...)\" failed. Error code %ld\n", 
					WSAGetLastError());
			} // end else recive failed

		} // end if new_connection
		
	} // end while

	// close the listening socket
	if (closesocket(listening_socket) != 0)
	{
		// error
		printf("Server: \"closesocket(...)\" falied! Error code %ld\n",
			WSAGetLastError());

		// do cleanup
		WSACleanup();

		// hold console for me before closing
		printf("\n\nPress any key to exit...\n");
		_getch();

		// back to windows
		return(-1);
	}
	// clean the dll from memory
	if (WSACleanup() != 0)
	{
		// error
		printf("Server: \"WSACleanup()\" falied! Error code %ld\n",
			WSAGetLastError());

		// hold console for me before closing
		printf("\n\nPress any key to exit...\n");
		_getch();

		// back to windows
		return(-1);
	} // end if cleanup

	// print exit message and quit app
	printf("\n\nPress any key to exit...\n");
	_getch();

	// back to OS
	return 0;

} // end main