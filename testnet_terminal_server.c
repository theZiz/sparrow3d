/*
 The contents of this file are subject to the "do whatever you like"-license.
 That means: Do, whatver you want, this file is under public domain. It is an
 example for sparrow3d. Copy it and learn from it for your project and release
 it under every license you want. ;-)
 For feedback and questions about my Files and Projects please mail me,
 Alexander Matthes (Ziz) , zizsdl_at_googlemail.com
*/
#include <sparrowNet.h>
#include <stdio.h>

spNetTCPServer server = NULL;
spNetTCPConnection server_connection = NULL;

void do_stuff()
{
	char command[256];
	printf("Press Ctrl+C to exit.\n");
	while (1)
	{
		if (server_connection == NULL)
		{
			server_connection = spNetAcceptTCP(server);
			if (server_connection)
			{
				spNetIP ip = spNetGetConnectionIP(server_connection);
				printf("Connection from %i.%i.%i.%i\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3]);
			}
		}
		if (server_connection != NULL)
		{
			int res;
			res = spNetReceiveTCP(server_connection,command,256);
			if (res == 0) //Connection was closed :(
			{
				spNetCloseTCP(server_connection);
				server_connection = NULL;
				printf("Connection was closed by client\n");
			}
			else
				printf("Received: \"%s\"\n",command);
		}
	}
}

int main( int argc, char **argv )
{
	spInitNet();
	printf("Init spNet\n");

	//Testing stuff ;)
	spNetIP ip = spNetResolve("ziz.gp2x.de",80);
	printf("IP of ziz.gp2x.de: %i.%i.%i.%i\n",ip.address.ipv4_bytes[0],ip.address.ipv4_bytes[1],ip.address.ipv4_bytes[2],ip.address.ipv4_bytes[3]);
	char buffer[256];
	printf("Host of the IP of ziz.gp2x.de:\"%s\"\n",spNetResolveHost(ip,buffer,256));
	printf("Open Connection to ziz.gp2x.de\n");
	server_connection = spNetOpenClientTCP(ip);
	spNetSendHTTP(server_connection,"GET /index.htm http/1.0\nHost: ziz.gp2x.de\n\n");
	int res = 1;
	while (res)
	{
		res = spNetReceiveHTTP(server_connection,buffer,255);
		buffer[res] = 0;
		printf("%s",buffer);
	}
	printf("Close Connection to ziz.gp2x.de\n");
	spNetCloseTCP(server_connection);
	server_connection = NULL;
	//Server setup
	server = spNetOpenServerTCP(12345);
	
	do_stuff();
	
	if (server_connection)
		spNetCloseTCP(server_connection);
	spNetCloseTCP(server);

	spQuitNet();
	return 0;
}
