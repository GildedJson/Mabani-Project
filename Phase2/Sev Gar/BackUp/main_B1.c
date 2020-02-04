#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <winsock2.h>
#include <windows.h>
#include "cJSON.h"
#include "cJSON.c"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

char *filePath = "files/";

struct useGar{
    char name[MAX];
    char tokenu[32];
};

int loggedInCount = 0;
struct useGar *logArray;
char buffer[5*MAX];
HANDLE hStdOut;


char *strAdd(int c, ...){
    va_list args;
    va_start(args, c);
    int len = 1;
    for(int i=0; i<c; i++){
        len += strlen(va_arg(args, char*));
    }
    va_end(args);
    char *resu = calloc(len,1);
    va_start(args, c);
    for(int i=0; i<c; i++){
        strcat(resu, va_arg(args, char*));
    }
    va_end(args);
    return resu;
}

void registGar(){
    char userName[MAX], password[MAX];
    sscanf(buffer, "%[^,], %s", userName, password);
    char *addr = strAdd(4, filePath, "user.", userName, ".json");
    FILE *currUser = fopen(addr, "r");
    if(currUser){
        cJSON *err = cJSON_CreateObject();
        cJSON_AddItemToObject(err, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(err, "content", cJSON_CreateString("User with this name already exists."));
        strcpy(buffer, cJSON_Print(err));
        cJSON_Delete(err);
    }
    else{
        fclose(currUser);
        currUser = fopen(addr, "w");
        cJSON *user = cJSON_CreateObject();
        cJSON_AddItemToObject(user, "username", cJSON_CreateString(userName));
        cJSON_AddItemToObject(user, "password", cJSON_CreateString(password));
        fprintf(currUser, "%s", cJSON_Print(user));
        cJSON_Delete(user);
        cJSON *succ = cJSON_CreateObject();
        cJSON_AddItemToObject(succ, "type", cJSON_CreateString("Successful"));
        cJSON_AddItemToObject(succ, "content", cJSON_CreateString(""));
        strcpy(buffer, cJSON_Print(succ));
        cJSON_Delete(succ);
    }
    fclose(currUser);
    free(addr);
}

void logIn(){
    char userName[MAX], password[MAX];
    sscanf(buffer, "%[^,], %s", userName, password);
    char *addr = strAdd(4, filePath, "user.", userName, ".json");
    FILE *currUser = fopen(addr, "r");
    if(currUser){
        char *bigBuff = calloc(500, 1);
        char c;
        for(int i=0; feof(currUser) == 0; i++){
            c = fgetc(currUser);
            strncat(bigBuff, &c, 1);
        }
        fclose(currUser);
        cJSON *curUser = cJSON_Parse(bigBuff);
        if(!strcmp((cJSON_GetObjectItem(curUser, "password") ->valuestring), password)){
            loggedInCount++;
            logArray = realloc(logArray, loggedInCount);
            strcpy(logArray[loggedInCount-1].name, userName);
            logArray[loggedInCount-1].tokenu[31] = 0;
            for(int i=0; i<31; i++){
                logArray[loggedInCount-1].tokenu[i] =
            }
        }
        else{
            cJSON *err = cJSON_CreateObject();
            cJSON_AddItemToObject(err, "type", cJSON_CreateString("Error"));
            cJSON_AddItemToObject(err, "content", cJSON_CreateString("Wrong password."));
            strcpy(buffer, cJSON_Print(err));
            cJSON_Delete(err);
        }
        cJSON_Delete(curUser);
    }
    else{
        cJSON *err = cJSON_CreateObject();
        cJSON_AddItemToObject(err, "type", cJSON_CreateString("Error"));
        cJSON_AddItemToObject(err, "content", cJSON_CreateString("User with this name doesn't exist."));
        strcpy(buffer, cJSON_Print(err));
        cJSON_Delete(err);
    }
}








// Function designed for chat between client and server.
void chat(int client_socket)
{
    char smallBuff[15] = {};
    int n;
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        n = 0;
        SetConsoleTextAttribute(hStdOut, 10);
        // Read the message from client and copy it to buffer
        /*recv(client_socket, buffer, sizeof(buffer), 0);*/
        while ((buffer[n++] = getchar()) != '\n')
            ;


        sscanf(buffer, "%s %[^\n]s", smallBuff, buffer);
        if(!strcmp(smallBuff, "register")){
            registGar();
        }
        else if(!strcmp(smallBuff, "register")){
            logIn();
        }

        // Print buffer which contains the client message
        printf("From client: %s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
        n = 0;
        // Copy server message to the buffer
        while ((buffer[n++] = getchar()) != '\n')
            ;



        // Send the buffer to client
        /*send(client_socket, buffer, sizeof(buffer), 0);*/
        printf("To client : %s\n", buffer);
        // If the message starts with "exit" then server exits and chat ends
        if (strncmp("exit", buffer, 4) == 0)
        {
            printf("Server stopping...\n");
            return;
        }
    }
}









// Driver function
int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server, client;
    logArray = malloc(0);
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

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
    server_socket = socket(AF_INET, SOCK_STREAM, 6);
	if (server_socket == INVALID_SOCKET)
        wprintf(L"socket function failed with error = %d\n", WSAGetLastError() );
    else
        printf("Socket successfully created..\n");

    // Assign IP and port
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    // Bind newly created socket to given IP and verify
    if ((bind(server_socket, (SA *)&server, sizeof(server))) != 0)
    {
        printf("Socket binding failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully bound..\n");

    // Now server is ready to listen and verify
    if ((listen(server_socket, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");

    // Accept the data packet from client and verify
    /*int len = sizeof(client);
    client_socket = accept(server_socket, (SA *)&client, &len);
    if (client_socket < 0)
    {
        printf("Server accceptance failed...\n");
        exit(0);
    }
    else
        printf("Server acccepted the client..\n");
*/
    // Function for chatting between client and server
    chat(client_socket);

    // Close the socket
    closesocket(server_socket);
}
