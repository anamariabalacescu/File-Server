#include "general_usage.h"

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

void new_parse_server_response(const char *server_message) {

    //printf("am primit ceva\n");
    uint32_t op;
    char operation[10];
    int i = 0;

    while(server_message[i]!=';')
    {
        //printf("%c\t", server_message[i]);
        operation[i] = server_message[i];
        //printf("%c\n", operation[i]);
        i++;
    }
    i++;
    printf("%s\n", operation);

    op = atoi(operation);
    printf("%x\n", op);

    // Parse the string
    if(op != 0)
    {
        printf("%x\n", op);
    }
    else {
        int numberOfFiles=0;
        //first we take number of bytes 
        char numberOfBytes[200];
        int lenNumberOfBytes=0;
        int totalLengthMessage=0;
        int index=0;
        while(server_message[i]!=';')
        {
            numberOfBytes[index]=server_message[i];
            i++;
            index++;
        }
        i++;
        
        numberOfBytes[index]='\0';
        
        int num_bytes=atoi(numberOfBytes);
        int j = 0;

        printf("Files in %s\n", SERVER_STORAGE);
        while(j < num_bytes)
        {
            if(server_message[i]!='\0')
                printf("%c", server_message[i]);
            else
                printf("\n");
            i++;
            j++;
        }
    }
}

void handle_option(uint32_t op, int socket_descriptor)
{
    char client_message[2000], server_message[2000];
    

    switch (op)
    {
        case 0x0:
            {
                sprintf(client_message,"%x", op);
                //printf("%s\n%d", client_message, socket_descriptor);
                //printf("suntem aici\n");
                if(send(socket_descriptor, client_message, strlen(client_message), 0) < 0) {
                    perror("Unable to send message.\n");
                    return -1;
                }

                //printf("suntem aici 2\n");

                //trimitem 0x0 si luam inapoi fisierele
                if(recv(socket_descriptor, server_message, sizeof(server_message), 0) < 0) {
                    perror("Error while receiving server's message.\n");
                    return -1;
                }

                //verify status
                               
                new_parse_server_response(server_message);

                memset(server_message, '\0', sizeof(client_message));
                //printf("aici: %s\n", server_message);

                //finally working
            }
            break;

        case 0x1:
        {
            printf("Select filepath to download: ");
            scanf("%s", &client_message);
            
        }
            break;

        case 0x2:
            break;

        case 0x4:
        {
            char filepath[MAX_PATH_LENGTH];
            printf("Select filepath to delete: ");
            scanf("%s", &filepath);
            sprintf(client_message, "%x;%zu;%s", op, strlen(filepath), filepath);
            if(send(socket_descriptor, client_message, strlen(client_message), 0) < 0) {
                perror("Unable to send message.\n");
                return -1;
            }
            //primim 0x0 => s-a sters, altfel esec
            if(recv(socket_descriptor, server_message, sizeof(server_message), 0) < 0) {
                perror("Error while receiving server's message.\n");
                return -1;
            }
            uint32_t code;
            sprintf(server_message, "%x", code);
            printf("%x\n", code);

        }
            break;

        case MOVE: //MOVE
            //cod_operație; nr_octeți_cale_fișier_sursă; cale_fișier_sursă_\0; nr_octeți_cale_fișier_destinație; cale_fișier_desitnație_\0
            char srcpath[MAX_PATH_LENGTH];
            char destpath[MAX_PATH_LENGTH];
            
            printf("Select filepath to move from: ");
            scanf("%s", &srcpath);
            printf("Select filepath to move to: ");
            scanf("%s", &destpath);

            uint32_t srcL = strlen(srcpath);
            uint32_t destL = strlen(destpath);
            char destLength[10];
            sprintf(destLength, "%x", destL);
            destLength[strlen(destLength)] = '\0';

            sprintf(client_message, "0x8;%x;%s", srcL, srcpath);
            int clientL = strlen(client_message);
            client_message[clientL] = '\0';
            clientL++;

            int i = 0;
            while(destLength[i] != '\0')
            {
                client_message[clientL] = destLength[i];
                clientL++;
                i++;
            }
            i = 0;
            while(i < destL)
            {
                client_message[clientL] = destpath[i];
                clientL++;
                i++;
            }
            client_message[clientL] = '\0';
            // for(i = 0; i <= clientL; i++)
            // {
            //     printf("%c", client_message[i]);
            // }

            if(send(socket_descriptor, client_message, strlen(client_message), 0) < 0) {
                perror("Unable to send message.\n");
                return -1;
            }
            //primim 0x0 => s-a sters, altfel esec
            if(recv(socket_descriptor, server_message, sizeof(server_message), 0) < 0) {
                perror("Error while receiving server's message.\n");
                return -1;
            }
            uint32_t code;
            sprintf(server_message, "%x", code);
            printf("%x\n", code);

            break;

        case 0x10:
            break;
        case 0x20:
            break;
        default:
            break;
    }
}

int main(int argc, char** argv)
{
    int socket_descriptor;
    struct sockaddr_in server_add;
    char client_message[2000], server_message[2000];

    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    // printf("aici\n");
    if(socket_descriptor < 0)
    {
        perror("Unable to create socket.\n");
        return -1;
    }
// printf("aici\n");
    printf("Socket created successfully.\n");

    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(SERVER_PORT);
    server_add.sin_addr.s_addr = inet_addr(SERVER_IP);
// printf("aici\n");
    if(connect(socket_descriptor, (struct sockaddr *) &server_add, sizeof(server_add)) < 0)
    {
        perror("Unable to connect\n");
        return -1;
    }
// printf("aici\n");
    printf("Connected with the server successfully.\n");

    client_menu();

    while(1) {
        memset(client_message, '\0', sizeof(client_message));
        printf("Enter option: ");

        uint32_t option;
        
        if (scanf("%x" , &option) != 1) {
            printf("Invalid input. Please enter a valid uint32_t value.\n");
            return 1; // Return an error code
        }
        // printf("Option: %x\n",option);
        handle_option(option, socket_descriptor);
    }

    close(socket_descriptor);
    
    return 0;
}
