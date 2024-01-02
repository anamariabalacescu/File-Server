#include "general_usage.h"

#include <stdio.h>
#include <string.h>

void client_menu()
{
    printf("\t\t\tChoose one option\n");
    printf("-> To LIST all files from the server send: %x\n", LIST);
    printf("-> To DOWNLOAD a file send: %x; number of bytes of the filepath; filepath\n", DOWNLOAD);
    printf("-> To UPLOAD a file send: %x; number of bytes of the filepath; filepath; number of bytes of the file's content; file content\n", UPLOAD);
    printf("-> To DELETE an existing file send: %x; number of bytes of the filepath; filepath\n", DELETE);
    printf("-> To MOVE an existing file send: %x; number of bytes of the source filepath; source filepath; number of bytes of the destination filepath; destination filepath\n", MOVE);
    printf("-> To UPDATE an existing file send: %x, number of bytes of the filepath; filepath; start byte; dimension; new characters\n", UPDATE);
    printf("-> To SEARCH for a word send: %x; bytes for word; word\n", SEARCH);
    printf("\n\t\t<--- PLEASE RESPECT THE FORMAT --->\n");
}

void parse_server_response(const char *server_message, int op) {
    int status, num_bytes;
    printf("%s\n", server_message);
    //printf("%d", op);
    if (op == 0) {
        char fileList[200][200];
        int numberOfFiles=0;

        printf("Status is okay as you are here :)\n");

        //first we take number of bytes 
        char numberOfBytes[200];
        int lenNumberOfBytes=0;
        int totalLengthMessage=0;
        int index=0;
        while(server_message[index]!='\0')
        {
            numberOfBytes[index]=server_message[index];
            index++;
        }
        numberOfBytes[index]='\0';
        printf("Number of bytes received: %s\n",numberOfBytes);
        num_bytes=atoi(numberOfBytes);
        index++;
        totalLengthMessage=index+num_bytes;
        while(index<totalLengthMessage)
        {
            fileList[numberOfFiles][lenNumberOfBytes]= server_message[index];
            index++;
            lenNumberOfBytes++;
            if(server_message[index]=='\0'){
                fileList[numberOfFiles][lenNumberOfBytes]='\0';
                numberOfFiles++;
                index++;
                lenNumberOfBytes=0;
            }
        }
        printf("File:\n");
        for(int i=0;i<numberOfFiles;i++){
            printf("%s\n",fileList[i]);
        }
        
    }
}

int main(int argc, char** argv)
{
    int socket_descriptor;
    struct sockaddr_in server_add;
    char client_message[2000], server_message[2000];

    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    printf("aici\n");
    if(socket_descriptor < 0)
    {
        perror("Unable to create socket.\n");
        return -1;
    }
printf("aici\n");
    printf("Socket created successfully.\n");

    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(SERVER_PORT);
    server_add.sin_addr.s_addr = inet_addr(SERVER_IP);
printf("aici\n");
    if(connect(socket_descriptor, (struct sockaddr *) &server_add, sizeof(server_add)) < 0)
    {
        perror("Unable to connect\n");
        return -1;
    }
printf("aici\n");
    printf("Connected with the server successfully.\n");

    client_menu();

    while(1) {
        memset(client_message, '\0', sizeof(client_message));
        printf("Enter option: ");
        
        if (fgets(client_message, sizeof(client_message), stdin) == NULL) {
            perror("Error reading input");
            break;
        }

        size_t len = strlen(client_message);
        if (len > 0 && client_message[len - 1] == '\n') {
            client_message[len - 1] = '\0';
        }

        int operation;
        sscanf(client_message, "%x", &operation);

        if(send(socket_descriptor, client_message, strlen(client_message), 0) < 0) {
            perror("Unable to send message.\n");
            return -1;
        }

        if(recv(socket_descriptor, server_message, sizeof(server_message), 0) < 0) {
            perror("Error while receiving server's message.\n");
            return -1;
        }

        printf("aici: %s\n", server_message);
        // if(operation == LIST)
        //     parse_server_response(server_message, operation);
        // else
        //     printf("%s", server_message);

    }

    close(socket_descriptor);
    
    return 0;
}
