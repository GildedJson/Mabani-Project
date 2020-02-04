#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <winsock2.h>
#include <windows.h>
#include <time.h>
#include "cJSON.h"
#include "cJSON.c"
#define MAX 80
#define PORT 12345
#define SA struct sockaddr

char *filePath = "files/";

struct useGar{
    char currChannelName[MAX];
    int currChannelSeenNum;
    char name[MAX];
    char tokenu[32];
};

int loggedInCount = 0;
struct useGar *logArray;
//char buffer[15*MAX];
char *buffer;
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

int checkAuth(char *authTok){
    if(authTok[0] != 0)
        for(int i=0; i<loggedInCount; i++){

            if(!strcmp(authTok, logArray[i].tokenu)){
                return i;
            }
        }
    return -1;
}

void errorBuffer(const char *errMsg){
    cJSON *err = cJSON_CreateObject();
    cJSON_AddItemToObject(err, "type", cJSON_CreateString("Error"));
    cJSON_AddItemToObject(err, "content", cJSON_CreateString(errMsg));
    strcpy(buffer, cJSON_Print(err));
    cJSON_Delete(err);
}

void emptySuccessBuffer(){
    cJSON *succ = cJSON_CreateObject();
    cJSON_AddItemToObject(succ, "type", cJSON_CreateString("Successful"));
    cJSON_AddItemToObject(succ, "content", cJSON_CreateString(""));
    strcpy(buffer, cJSON_Print(succ));
    cJSON_Delete(succ);
}

void addMessageToChannel(const char *sender, const char *message, const char *channelName){
    char *addr = strAdd(4, filePath, "channel.", channelName, ".json");
    FILE *currChannel = fopen(addr, "r");
    if(currChannel){
        fseek(currChannel, 0, SEEK_END);
        int len = ftell(currChannel);
        fseek(currChannel, 0, SEEK_SET);
        char *channelData = calloc(len+1,1);
        char c;
        for(int i=0; feof(currChannel) == 0; i++){
            c = fgetc(currChannel);
            strncat(channelData, &c, 1);
        }
        fclose(currChannel);
        currChannel = fopen(addr, "w");
        cJSON *chan = cJSON_Parse(channelData);
        free(channelData);
        //printf("\nHellish: %s\n", cJSON_Print(cJSON_GetObjectItem(chan, "content")));
        cJSON *madeMes = cJSON_CreateObject();
        cJSON_AddItemToObject(madeMes, "sender", cJSON_CreateString(sender));
        cJSON_AddItemToObject(madeMes, "content", cJSON_CreateString(message));
        cJSON_AddItemToArray(cJSON_GetObjectItem(chan, "messages"), madeMes);
        fprintf(currChannel, "%s", cJSON_Print(chan));
        cJSON_Delete(chan);
    }
    free(addr);
    fclose(currChannel);
}

void registGar(){
    char userName[MAX], password[MAX];
    sscanf(buffer, "%[^,], %s", userName, password);
    char *addr = strAdd(4, filePath, "user.", userName, ".json");
    FILE *currUser = fopen(addr, "r");
    if(currUser){
        errorBuffer("User with this name already exists.");
    }
    else if(!strncmp(userName, "Server", 6)){
        errorBuffer("Username can\'t start with \"Server\".");
    }
    else{
        fclose(currUser);
        currUser = fopen(addr, "w");
        cJSON *user = cJSON_CreateObject();
        cJSON_AddItemToObject(user, "username", cJSON_CreateString(userName));
        cJSON_AddItemToObject(user, "password", cJSON_CreateString(password));
        fprintf(currUser, "%s", cJSON_Print(user));
        cJSON_Delete(user);
        emptySuccessBuffer();
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
            printf("\nPoison: %d, %d", sizeof(logArray), loggedInCount);
            logArray = realloc(logArray, loggedInCount * sizeof(struct useGar));
            //Set name, token and such:
            strcpy(logArray[loggedInCount-1].name, userName);
            logArray[loggedInCount-1].tokenu[31] = 0;
            logArray[loggedInCount-1].currChannelSeenNum = 0;
            logArray[loggedInCount-1].currChannelName[0] = 0;
            srand(time(NULL));
            do{
                for(int i=0; i<31; i++){
                    logArray[loggedInCount-1].tokenu[i] = 48 + rand() * 42 / RAND_MAX;
                }
            }while(checkAuth(logArray[loggedInCount-1].tokenu) != loggedInCount-1);
            cJSON *succa = cJSON_CreateObject();
            cJSON_AddItemToObject(succa, "type", cJSON_CreateString("AuthToken"));
            cJSON_AddItemToObject(succa, "content", cJSON_CreateString(logArray[loggedInCount-1].tokenu));
            strcpy(buffer, cJSON_Print(succa));
            cJSON_Delete(succa);
        }
        else{
            errorBuffer("Wrong password.");
        }
        //printf("\ni'm near\n");
        cJSON_Delete(curUser);
        //printf("\ni'm rear\n");
    }
    else{
        fclose(currUser);
        errorBuffer("User with this name doesn\'t exist.");
    }
    free(addr);
}

void createChannel(){
    char channelName[MAX], authTok[32];
    sscanf(buffer, "%[^,], %s", channelName, authTok);
    int userNum = checkAuth(authTok);
    if(userNum != -1){
        char *addr = strAdd(4, filePath, "channel.", channelName, ".json");
        FILE *currChannel = fopen(addr, "r");
        if(currChannel){
            errorBuffer("This channel already exists.");
        }
        else{
            fclose(currChannel);
            currChannel = fopen(addr, "w");
            cJSON *chan = cJSON_CreateObject();
            cJSON_AddArrayToObject(chan, "messages");
            cJSON *madeMes = cJSON_CreateObject();
            cJSON_AddItemToObject(madeMes, "sender", cJSON_CreateString("Server.c"));
            char *temp = strAdd(2, logArray[userNum].name, " created this channel.");
            cJSON_AddItemToObject(madeMes, "content", cJSON_CreateString(temp));
            cJSON_AddItemToArray(cJSON_GetObjectItem(chan,"messages"), madeMes);
            fprintf(currChannel, "%s", cJSON_Print(chan));
            free(temp);
            cJSON_Delete(chan);
            emptySuccessBuffer();
        }
        fclose(currChannel);
        free(addr);
    }
    else{
        errorBuffer("Wrong token.");
    }
}

void joinChannel(){
    char channelName[MAX], authTok[32];
    sscanf(buffer, "%[^,], %s", channelName, authTok);
    int userNum = checkAuth(authTok);
    if(userNum != -1){
        char *addr = strAdd(4, filePath, "channel.", channelName, ".json");
        FILE *currChannel = fopen(addr, "r");
        if(!currChannel){
            errorBuffer("This channel doesn\'t exists.");
        }
        else{
            char *temp = strAdd(2, logArray[userNum].name, " joined the channel.");
            addMessageToChannel("Server.j", temp, channelName);
            //Change the user's channel name
            strcpy(logArray[userNum].currChannelName, channelName);
            logArray[userNum].currChannelSeenNum = 0;
            printf("\nset %d to %d\n", userNum, logArray[userNum].currChannelSeenNum);
            emptySuccessBuffer();
        }
        fclose(currChannel);
        free(addr);
    }
    else{
        errorBuffer("Wrong token.");
    }
}

void sendAMessage(){
    char messa[2*MAX], authTok[32];
    sscanf(buffer, "%[^,], %s", messa, authTok);
    int userNum = checkAuth(authTok);
    if(userNum != -1){
        if(logArray[userNum].currChannelName[0]){
            addMessageToChannel(logArray[userNum].name, messa, logArray[userNum].currChannelName);
            emptySuccessBuffer();
        }
        else{
            errorBuffer("Impossible error (you\'re in no channel).");
        }
    }
    else{
        errorBuffer("Wrong token.");
    }
}

void refreshing(){
    int userNum = checkAuth(buffer);
    if(userNum != -1){
        if(logArray[userNum].currChannelName[0]){
            cJSON *listi = cJSON_CreateObject();
            cJSON_AddItemToObject(listi, "type", cJSON_CreateString("List"));
            cJSON_AddArrayToObject(listi, "content");

            char *addr = strAdd(4, filePath, "channel.", logArray[userNum].currChannelName, ".json");
            FILE *currChannel = fopen(addr, "r");
            free(addr);
            fseek(currChannel, 0, SEEK_END);
            int len = ftell(currChannel);
            fseek(currChannel, 0, SEEK_SET);
            char *channelData = calloc(len+1,1);
            char c;
            for(int i=0; feof(currChannel) == 0; i++){
                c = fgetc(currChannel);
                strncat(channelData, &c, 1);
            }
            fclose(currChannel);
            cJSON *chan = cJSON_Parse(channelData);
            free(channelData);
            int counter = cJSON_GetArraySize(cJSON_GetObjectItem(chan, "messages"));


            cJSON_AddItemToArray(cJSON_GetObjectItem(listi, "content"), cJSON_GetArrayItem(cJSON_GetObjectItem(chan, "messages"), logArray[userNum].currChannelSeenNum));
            printf("\nwhat num: %d from %d", logArray[userNum].currChannelSeenNum, userNum);


            /*for(int i=logArray[userNum].currChannelSeenNum; i<counter; i++){
                printf("\nok: %d from %d ", i, counter);
                cJSON *temple = cJSON_Duplicate(cJSON_GetArrayItem(tempObj, i), false);
                printf("\nHell: %s", cJSON_Print(temple));
                printf("\nBefore: %s", cJSON_Print(listi));
                cJSON_AddItemToArray(cJSON_GetObjectItem(listi, "content"), temple);
                printf("\nAfter: %s", cJSON_Print(listi));
                cJSON_Delete(temple);
            }*/
            logArray[userNum].currChannelSeenNum = counter;
            strcpy(buffer, cJSON_Print(listi));
            //cJSON_Delete(listi);
            //cJSON_Delete(chan);
        }
        else{
            errorBuffer("Impossible error (you\'re in no channel).");
        }
    }
    else{
        errorBuffer("Wrong token.");
    }
}













// Function designed for chat between client and server.
void chat(int client_socket)
{
    buffer = malloc(15*MAX);
    char smallBuff[15] = {};
    int n;
    //*****************
    strcpy(buffer, "2, 2");
    logIn();
    strcpy(buffer, strAdd(2, "fool, ", logArray[0].tokenu));
    joinChannel();
    printf("\nalo: %s\n", logArray[0].tokenu);
    //*****************
    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        memset(smallBuff, 0, sizeof(smallBuff));
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
        else if(!strcmp(smallBuff, "login")){
            logIn();
        }
        else if(!strcmp(smallBuff, "create")){
            sscanf(buffer, "%s %[^\n]s", smallBuff, buffer);
            createChannel();
        }
        else if(!strcmp(smallBuff, "join")){
            sscanf(buffer, "%s %[^\n]s", smallBuff, buffer);
            joinChannel();
        }
        else if(!strcmp(smallBuff, "send")){
            sendAMessage();
        }
        else if(!strcmp(smallBuff, "refresh")){
            refreshing();
        }

        // Print buffer which contains the client message
        printf("From client: %s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
        n = 0;
        // Copy server message to the buffer
        /*while ((buffer[n++] = getchar()) != '\n')
            ;*/



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
