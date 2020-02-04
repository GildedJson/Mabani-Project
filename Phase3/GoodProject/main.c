#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "cJSON.h"
#include "cJSON.c"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr


int client_socket, server_socket;
struct sockaddr_in servaddr, cli;
char takenToken[MAX]={};
char amILoggedIn = 0, amIInChannel = 0;
HANDLE hStdOut;

// Function designed for chat between client and server.
void chat(int server_socket)
{
	char buffer[MAX];
	int n;
	while (true) {
		memset(buffer, 0, sizeof(buffer));
		printf("Enter your message: ");
		n = 0;

		// Copy client message to the buffer
		while ((buffer[n++] = getchar()) != '\n')
			;

		// Send the buffer to server
		send(server_socket, buffer, sizeof(buffer), 0);
		memset(buffer, 0, sizeof(buffer));

		// Read the message from server and copy it to buffer
		recv(server_socket, buffer, sizeof(buffer), 0);

		// Print buffer which contains the server message
		printf("From server: %s", buffer);

		// If the message starts with "exit" then client exits and chat ends
		if ((strncmp(buffer, "exit", 4)) == 0) {
			printf("Client stopping...\n");
			return;
		}
	}
}

//int client_socket, struct sockaddr_in servaddr
void makeConnectSocket(){
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("-Socket creation failed...\n");
		exit(0);
	}
	/*else
		printf("-Socket successfully created...\n");*/
    if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("-Connection to the server failed...\n");
		exit(0);
	}
	/*else
		printf("-Successfully connected to the server...\n");*/
}
void connectSocket(){
    if (connect(client_socket, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("-Connection to the server failed...\n");
		exit(0);
	}
	else
		printf("-Successfully connected to the server...\n");
}

void registGar(){
    char useGar[MAX], passGar[MAX]={}, buff[2*MAX];
    SetConsoleTextAttribute(hStdOut, 42);
    printf("+");
    SetConsoleTextAttribute(hStdOut, 47);
    printf("Register New User\nEnter your esm:\n");
    SetConsoleTextAttribute(hStdOut, 15);
    scanf("%[^\n]s", useGar);
    getchar();
    SetConsoleTextAttribute(hStdOut, 47);
    printf("Enter your ramz:\n");
    SetConsoleTextAttribute(hStdOut, 15);
    scanf("%[^\n]s", passGar);
    getchar();
    passGar[strlen(passGar)] = 0;
    sprintf(buff, "register %s, %s\n", useGar, passGar);
    makeConnectSocket();
    send(client_socket, buff, sizeof(buff), 0);
    memset(buff, 0, sizeof(buff));
    recv(client_socket, buff, sizeof(buff), 0);
    closesocket(client_socket);

    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSomething went wrong:\n%s", cJSON_GetObjectItem(rice, "content") ->valuestring);
    }
    else{
        SetConsoleTextAttribute(hStdOut, 160);
        printf("\nRegistered");
    }
}

void logIn(){
    char useGar[MAX], passGar[MAX]={}, buff[2*MAX];
    SetConsoleTextAttribute(hStdOut, 42);
    printf("+");
    SetConsoleTextAttribute(hStdOut, 47);
    printf("Login\nEnter your esm:\n");
    SetConsoleTextAttribute(hStdOut, 15);
    scanf("%[^\n]s", useGar);
    getchar();
    SetConsoleTextAttribute(hStdOut, 47);
    printf("Enter your ramz:\n");
    SetConsoleTextAttribute(hStdOut, 15);
    scanf("%[^\n]s", passGar);
    getchar();
    sprintf(buff, "login %s, %s\n", useGar, passGar);
    passGar[strlen(passGar)] = 0;
    makeConnectSocket();
    send(client_socket, buff, sizeof(buff), 0);
    memset(buff, 0, sizeof(buff));
    recv(client_socket, buff, sizeof(buff), 0);
    closesocket(client_socket);

    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSomething went wrong:\n%s", cJSON_GetObjectItem(rice, "content") ->valuestring);
    }
    else{
        SetConsoleTextAttribute(hStdOut, 160);
        printf("\nWelcome %s", useGar);
        strcpy(takenToken, cJSON_GetObjectItem(rice, "content") ->valuestring);
        amILoggedIn = 1;
        //printf("\nToken: %s", cJSON_GetObjectItem(rice, "content") ->valuestring);
    }
}

void createJoinChannel(){
    char channelName[MAX], buff[2*MAX];
    SetConsoleTextAttribute(hStdOut, 93);
    printf("+");
    SetConsoleTextAttribute(hStdOut, 95);
    printf("Make New Channel\nEnter your channel\'s esm:\n");
    SetConsoleTextAttribute(hStdOut, 15);
    scanf("%[^\n]s", channelName);
    getchar();
    sprintf(buff, "create channel %s, %s\n", channelName, takenToken);
    makeConnectSocket();
    send(client_socket, buff, sizeof(buff), 0);
    memset(buff, 0, sizeof(buff));
    recv(client_socket, buff, sizeof(buff), 0);
    closesocket(client_socket);
    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSomething went wrong:\n%s", cJSON_GetObjectItem(rice, "content") ->valuestring);
    }
    else{
        SetConsoleTextAttribute(hStdOut, 80);
        printf("\nChannel \"%s\" is ready", channelName);
        amIInChannel = 1;
    }
}

void joinChannel(){
    char channelName[MAX], buff[2*MAX];
    SetConsoleTextAttribute(hStdOut, 93);
    printf("+");
    SetConsoleTextAttribute(hStdOut, 95);
    printf("Join Channel\nEnter channel\'s esm:\n");
    SetConsoleTextAttribute(hStdOut, 15);
    scanf("%[^\n]s", channelName);
    getchar();
    sprintf(buff, "join channel %s, %s\n", channelName, takenToken);
    makeConnectSocket();
    send(client_socket, buff, sizeof(buff), 0);
    memset(buff, 0, sizeof(buff));
    recv(client_socket, buff, sizeof(buff), 0);
    closesocket(client_socket);
    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSomething went wrong:\n%s", cJSON_GetObjectItem(rice, "content") ->valuestring);
    }
    else{
        SetConsoleTextAttribute(hStdOut, 80);
        printf("\nChannel \"%s\" is ready", channelName);
        amIInChannel = 1;
    }
}

void logOut(){
    char buff[2*MAX];
    sprintf(buff, "logout %s\n", takenToken);
    makeConnectSocket();
    send(client_socket, buff, sizeof(buff), 0);
    memset(buff, 0, sizeof(buff));
    recv(client_socket, buff, sizeof(buff), 0);
    closesocket(client_socket);
    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSadly the token is lost");
    }
    else{
        SetConsoleTextAttribute(hStdOut, 80);
        printf("\nGoodbye");
        amILoggedIn = 0;
    }
}

void sendMess(){
    char mess[MAX], buff[2*MAX];
    SetConsoleTextAttribute(hStdOut, 110);
    printf("+");
    SetConsoleTextAttribute(hStdOut, 111);
    printf("New Message\nEnter your message:\n");
    SetConsoleTextAttribute(hStdOut, 15);
    scanf("%[^\n]s", mess);
    getchar();
    sprintf(buff, "send %s, %s\n", mess, takenToken);
    makeConnectSocket();
    send(client_socket, buff, sizeof(buff), 0);
    memset(buff, 0, sizeof(buff));
    recv(client_socket, buff, sizeof(buff), 0);
    closesocket(client_socket);
    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSomething went wrong:\n%s", cJSON_GetObjectItem(rice, "content") ->valuestring);
    }
    else{
        SetConsoleTextAttribute(hStdOut, 224);
        printf("\nMessage sent");
    }
}

void refresh(){
    char *buff = malloc(100*MAX*sizeof(char));
    sprintf(buff, "refresh %s\n", takenToken);
    makeConnectSocket();
    send(client_socket, buff, 100*MAX*sizeof(char), 0);
    memset(buff, 0, 100*MAX*sizeof(char));
    recv(client_socket, buff, 100*MAX*sizeof(char), 0);
    closesocket(client_socket);
    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSomething went wrong:\n%s", cJSON_GetObjectItem(rice, "content") ->valuestring);
    }
    else{
        cJSON *polo = cJSON_GetObjectItem(rice, "content");
        SetConsoleTextAttribute(hStdOut, 110);
        printf("\nList of messages:");
        int poloSize = cJSON_GetArraySize(polo), maxiTool = 1;
        for(int i=0; i<poloSize; i++){
            cJSON *berenj = cJSON_GetArrayItem(polo, i);
            maxiTool = max(maxiTool, strlen(cJSON_GetObjectItem(berenj, "sender")->valuestring));
        }
        for(int i=0; i<poloSize; i++){
            cJSON *berenj = cJSON_GetArrayItem(polo, i);
            if(!strcmp(cJSON_GetObjectItem(berenj, "sender")->valuestring, "server")){
                SetConsoleTextAttribute(hStdOut, 11);
            }
            else if(!strcmp(cJSON_GetObjectItem(berenj, "sender")->valuestring, "Server.c")){
                SetConsoleTextAttribute(hStdOut, 10);
            }
            else if(!strcmp(cJSON_GetObjectItem(berenj, "sender")->valuestring, "Server.j")){
                SetConsoleTextAttribute(hStdOut, 11);
            }
            else if(!strcmp(cJSON_GetObjectItem(berenj, "sender")->valuestring, "Server.l")){
                SetConsoleTextAttribute(hStdOut, 12);
            }
            else{
                SetConsoleTextAttribute(hStdOut, 14);
            }
            printf("\n\t#%s", cJSON_GetObjectItem(berenj, "sender")->valuestring);
            if(!strcmp(cJSON_GetObjectItem(berenj, "sender")->valuestring, "server")){
                SetConsoleTextAttribute(hStdOut, 3);
            }
            else if(!strcmp(cJSON_GetObjectItem(berenj, "sender")->valuestring, "Server.c")){
                SetConsoleTextAttribute(hStdOut, 2);
            }
            else if(!strcmp(cJSON_GetObjectItem(berenj, "sender")->valuestring, "Server.j")){
                SetConsoleTextAttribute(hStdOut, 3);
            }
            else if(!strcmp(cJSON_GetObjectItem(berenj, "sender")->valuestring, "Server.l")){
                SetConsoleTextAttribute(hStdOut, 4);
            }
            else{
                SetConsoleTextAttribute(hStdOut, 6);
            }
            printf(":");
            for(int j=0; j<=(maxiTool - strlen(cJSON_GetObjectItem(berenj, "sender")->valuestring)); j++)
                printf(" ");
            SetConsoleTextAttribute(hStdOut, 15);
            printf("%s", cJSON_GetObjectItem(berenj, "content")->valuestring);
        }
        printf("\n");
    }
    free(buff);
}

void channelMembers(){
    char *buff = malloc(100*MAX*sizeof(char));
    sprintf(buff, "channel members %s\n", takenToken);
    makeConnectSocket();
    send(client_socket, buff, 100*MAX*sizeof(char), 0);
    memset(buff, 0, 100*MAX*sizeof(char));
    recv(client_socket, buff, 100*MAX*sizeof(char), 0);
    closesocket(client_socket);
    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSomething went wrong:\n%s", cJSON_GetObjectItem(rice, "content") ->valuestring);
    }
    else{
        cJSON *polo = cJSON_GetObjectItem(rice, "content");
        SetConsoleTextAttribute(hStdOut, 110);
        printf("\nList of channel members:");
        for(int i=0; i<cJSON_GetArraySize(polo); i++){
            SetConsoleTextAttribute(hStdOut, 6);
            printf("\n\t#");
            SetConsoleTextAttribute(hStdOut, 14);
            printf("%d", i+1);
            SetConsoleTextAttribute(hStdOut, 6);
            printf(": ");
            SetConsoleTextAttribute(hStdOut, 15);
            printf("%s", cJSON_GetArrayItem(polo, i)->valuestring);
        }
        printf("\n");
    }
    free(buff);
}

void leaveChannel(){
    char buff[2*MAX];
    sprintf(buff, "leave %s\n", takenToken);
    makeConnectSocket();
    send(client_socket, buff, sizeof(buff), 0);
    memset(buff, 0, sizeof(buff));
    recv(client_socket, buff, sizeof(buff), 0);
    closesocket(client_socket);
    cJSON *rice = cJSON_Parse(buff);
    if(!strcmp((cJSON_GetObjectItem(rice, "type") ->valuestring), "Error")){
        SetConsoleTextAttribute(hStdOut, 192);
        printf("\nSadly the token is lost");
    }
    else{
        SetConsoleTextAttribute(hStdOut, 224);
        printf("\nGoodluck");
        amIInChannel = 0;
    }
}







int main()
{

	WORD wVersionRequested;
    WSADATA wsaData;
    int err;

	// Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        // Tell the user that we could not find a usable Winsock DLL.
        printf("WSAStartup failed with error: %d\n", err);
        return 1;
    }

	// Create and verify socket
	/*client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == -1) {
		printf("Socket creation failed...\n");
		exit(0);
	}
	else
		printf("Socket successfully created..\n");
    */
    printf("In the name of GOD");
	// Assign IP and port
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

    char currentCommand[3] = {};
	while(currentCommand[0] != '3'){
        SetConsoleTextAttribute(hStdOut, 10);
        printf("\n==================\nAccount menu:");
        SetConsoleTextAttribute(hStdOut, 2);
        printf("\n1:");
        SetConsoleTextAttribute(hStdOut, 15);
        printf(" Register");
        SetConsoleTextAttribute(hStdOut, 2);
        printf("\n2:");
        SetConsoleTextAttribute(hStdOut, 15);
        printf(" Login");
        SetConsoleTextAttribute(hStdOut, 2);
        printf("\n3:");
        SetConsoleTextAttribute(hStdOut, 15);
        printf(" End\n");
        scanf("%c", &currentCommand[0]);
        if(currentCommand[0] != '\n')
            getchar();
        if(currentCommand[0] == '1')
            registGar();
        if(currentCommand[0] == '2')
            logIn();
        while(amILoggedIn == 1){
            SetConsoleTextAttribute(hStdOut, 13);
            printf("\n==================\nMain menu:");
            SetConsoleTextAttribute(hStdOut, 5);
            printf("\n1:");
            SetConsoleTextAttribute(hStdOut, 15);
            printf(" Create Channel");
            SetConsoleTextAttribute(hStdOut, 5);
            printf("\n2:");
            SetConsoleTextAttribute(hStdOut, 15);
            printf(" Join Channel");
            SetConsoleTextAttribute(hStdOut, 5);
            printf("\n3:");
            SetConsoleTextAttribute(hStdOut, 15);
            printf(" Logout\n");
            scanf("%c", &currentCommand[1]);
            if(currentCommand[1] != '\n')
                getchar();
            if(currentCommand[1] == '1')
                createJoinChannel();
            if(currentCommand[1] == '2')
                joinChannel();
            if(currentCommand[1] == '3')
                logOut();
            while(amIInChannel == 1){
                SetConsoleTextAttribute(hStdOut, 14);
                printf("\n==================\nChat menu:");
                SetConsoleTextAttribute(hStdOut, 6);
                printf("\n1:");
                SetConsoleTextAttribute(hStdOut, 15);
                printf(" Send Message");
                SetConsoleTextAttribute(hStdOut, 6);
                printf("\n2:");
                SetConsoleTextAttribute(hStdOut, 15);
                printf(" Refresh");
                SetConsoleTextAttribute(hStdOut, 6);
                printf("\n3:");
                SetConsoleTextAttribute(hStdOut, 15);
                printf(" Channel members");
                SetConsoleTextAttribute(hStdOut, 6);
                printf("\n4:");
                SetConsoleTextAttribute(hStdOut, 15);
                printf(" leave Channel\n");
                scanf("%c", &currentCommand[2]);
                if(currentCommand[2] != '\n')
                    getchar();
                if(currentCommand[2] == '1')
                    sendMess();
                if(currentCommand[2] == '2')
                    refresh();
                if(currentCommand[2] == '3')
                    channelMembers();
                if(currentCommand[2] == '4')
                    leaveChannel();
            }
        }
	}

	// Function for chat
	//chat(client_socket);

	// Close the socket
	//closesocket(client_socket);
}
