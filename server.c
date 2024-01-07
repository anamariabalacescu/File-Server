#include "general_usage.h"


typedef struct file_data{
    char *name;
    char **top10;  /*cele mai cautate 10 cuvinte*/
    int *count10;
    int in10;

    char **words;
    int *word_count;
    int words_total;
}file_data;

file_data *files = NULL;
int file_number = 0;

void initializeFiles()
{
    files = (file_data*) malloc(sizeof(file_data) * 10);
    for(int i = 0; i < 10; i++)
    {
        files[i].name = NULL;
        files[i].count10 = (int*) malloc(sizeof(int) * 10);
        files[i].in10 = 0;  // Corrected the typo
        files[i].top10 = (char**)malloc(sizeof(char*) * 10);
        for (int j = 0; j < 10; j++) {
            files[i].top10[j] = NULL;
        }

        files[i].words = (char**)malloc(sizeof(char*) * 10);
        for (int j = 0; j < 10; j++) {
            files[i].words[j] = NULL;
        }
        files[i].words_total = 0;
        files[i].word_count = (int*) malloc(sizeof(int) * 10);
    }
}

void freeFiles()
{
    for(int i = 0; i < file_number; i++)
    {
        free(files[i].name);
        
        for(int j = 0; j < files[i].in10; j++)
        {
            free(files[i].top10[j]);
        }
        free(files[i].top10);
        free(files[i].count10);

        for(int j = 0; j < files[i].words_total; j++)
        {
            free(files[i].words[j]);
            free(files[i].word_count[j]);
        }
        free(files[i].words);
        free(files[i].word_count);
    }
    free(files);
}

int removeFile(char *filename) {
    int i, found = 0;

    // Find the index of the file in the array
    for (i = 0; i < file_number; i++) {
        if (files[i].name != NULL && strcmp(files[i].name, filename) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        // Free memory for the file data
        free(files[i].name);

        for (int j = 0; j < files[i].in10; j++) {
            free(files[i].top10[j]);
        }
        free(files[i].top10);
        free(files[i].count10);

        for (int j = 0; j < files[i].words_total; j++) {
            free(files[i].words[j]);
        }
        free(files[i].words);
        free(files[i].word_count);

        for (int j = i; j < file_number - 1; j++) {
            files[j] = files[j + 1];
        }

        file_number--;

        // Resize the files array
        files = realloc(files, sizeof(file_data) * file_number);

        return 1; // Success
    } else {
        return 0; // File not found
    }
}

int addFile(char *filename)
{

    files[file_number].name = (char*) malloc (sizeof(char) * strlen(filename));
    strcpy(files[file_number].name, filename);
    files[file_number].name[strlen(filename)] = NULL;
    file_number++;
}

void listFilesRecursively(char *basePath, file_data *files, int *index) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(basePath)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {  // Check if it's a regular file
                // Allocate memory for the file_data structure
                files[*index].name = (char *)malloc(strlen(basePath) + strlen(ent->d_name) + 3);  // +1 for '/', +1 for '\0'
                snprintf(files[*index].name, strlen(basePath) + strlen(ent->d_name) + 2, "%s/%s", basePath, ent->d_name);
                files[*index].name[strlen(basePath) + strlen(ent->d_name) + 2] = '\0';

                // Initialize other fields in file_data
                files[*index].top10 = NULL;  // Initialize accordingly
                files[*index].count10 = NULL;  // Initialize accordingly
                files[*index].in10 = 0;
                files[*index].words = NULL;  // Initialize accordingly
                files[*index].word_count = NULL;  // Initialize accordingly
                files[*index].words_total = 0;

                (*index)++;
            } else if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                // Recursive call for directories (skip '.' and '..')
                char path[2000];
                snprintf(path, sizeof(path), "%s/%s", basePath, ent->d_name);
                //path[strlen(basePath) + strlen(ent->d_name) + 1] = '\0';
                listFilesRecursively(path, files, index);
            }
        }
        closedir(dir);
    }
}

void addAllFiles() {
    // Adjust the size accordingly
    int index = 0;

    listFilesRecursively(SERVER_STORAGE, files, &index);
    file_number = index;
    // Print or process the file_data structures as needed
    for (int i = 0; i < index; i++) {
        printf("File Name: %s\n", files[i].name);
    }
}

int searchForFile(char *filename)
{
    if(file_number < 0)
        return 0; //no files
    
    printf("%s1\n", filename);

    int i = 0;
    for(int i = 0; i < file_number; i++)
    {
        printf("%d", i);
        if(strstr(files[i].name, filename)!=NULL)
            return i;
    }
    return -1;
}

int inTop10(char *filename, char *word)
{
    int i = 0;
    while(i < file_number) {
        if(strcmp(files[i].name, filename) == 0)
        {
            int j = 0;
            while(files[i].top10[j] != NULL) {
                if(strcmp(files[i].top10[j], word) == 0) {
                    return j;
                }
            }
        }
        i++;
    }
    return -1;
}

void addCountTop10(char *filename, char *word, int p)
{
    int i = 0;
    int ok = 0;
    while(i < file_number && ok == 0) {
        if(strcmp(files[i].name, filename) == 0)
        {
            files[i].count10[p]++;
            ok = 1;
        }
        i++;
    }
}

void addTop10(char* filename, char *word, int appearances)
{
    int i = 0;
    while(strcmp(files[i].name, filename)!=0) i++;

    if(strcmp(files[i].name, filename) == 0)
    {
        if(files[i].in10 < 10) {
            files[i].in10++;
            int x = files[i].in10;
            files[i].top10[x] = (char*) malloc(sizeof(char) * (strlen(word) + 1));
            strcpy(files[i].top10[x], word);
            files[i].top10[x][strlen(word)] = '\n';
        }
        else {
            int x = files[i].in10;
            int y = x;
            int ok = 0;
            while(x > -1 && ok == 0)
            {
                if(files[i].count10[x] > appearances)
                    ok = 1;
                else
                    x--;
            }
            for(int j = y; j > x; j--)
            {
                strcpy(files[i].top10[j], files[i].top10[j-1]);
                files[i].count10[j] = files[i].count10[j-1];
            }
            strcpy(files[i].top10[x], word);
            files[i].count10[x] = appearances; 
        }
    }
}

int inBigList(char *filename, char *word)
{
    int i = 0;
    while(i < file_number) {
        if(strcmp(files[i].name, filename) == 0)
        {
            int j = 0;
             while(files[i].words[j] != NULL) {
                if(strcmp(files[i].words[j], word) == 0) {
                    return j;
                }
            }
        }
        i++;
    }
    return -1;
}

void add_count_word(char *filename, char *word, int p)
{
    int i = 0;
    int ok = 0;
    while(i < file_number && ok == 0){
        if(strcmp(files[i].name, filename) == 0)
        {
            files[i].word_count[p]++;
            ok = 1;
        }
        i++;
    } 
}

void addBigList(char* filename, char *word, int appearances)
{
    int i = 0;
    while(strcmp(files[i].name, filename)!=0) i++;

    if(strcmp(files[i].name, filename) == 0)
    {
        if(files[i].in10 < 10) {
            files[i].in10++;
            int x = files[i].in10;
            files[i].top10[x] = (char*) malloc(sizeof(char) * (strlen(word) + 1));
            strcpy(files[i].top10[x], word);
            files[i].top10[x][strlen(word)] = '\n';
        }
        else {
            int x = files[i].in10;
            int y = x;
            int ok = 0;
            while(x > -1 && ok == 0)
            {
                if(files[i].count10[x] > appearances)
                    ok = 1;
                else
                    x--;
            }
            for(int j = y; j > x; j--)
            {
                strcpy(files[i].top10[j], files[i].top10[j-1]);
                files[i].count10[j] = files[i].count10[j-1];
            }
            strcpy(files[i].top10[x], word);
            files[i].count10[x] = appearances; 
        }
    }
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

char* my_strcat(char* destination, const char* source) {
    char* result = destination;
    printf("Desti ini%s\n", destination);
    // Move to the end of the destination string
    while (*destination != '\0') {
        destination++;
    }
    destination++;

    // Copy the characters from the source to the destination
    while (*source != '\0') {
        *destination = *source;
        destination++;
        source++;
    }
    printf("Desti After%s\n", destination);
    // Add null terminator to the concatenated string
    *destination = '\0';
    printf("Desti AAAA%s\n", destination);
    return result;
}

char *semicol_strtok(char *start, int *startpoz) //kind of strtok
{
    char aux[10]; //used for op number and number of bytes;
    int j = 0;
    int ok = 0;
    int i = 0;
    for(i = *(startpoz); i < strlen(start) && ok == 0; i++)
        if(start[i] != ';')
        {
            aux[j] = start[i];
            j++;
        }
        else
            ok = 1;
    aux[j] = '\n';
    *(startpoz) = i + 2;
    return aux;
}

void checkFiles()
{
    printf("Files:\n");
    for(int i = 0; i < file_number; i++)
        printf("%s\n", files[i].name);
}

int deleteFile(const char *filepath) {
    if (unlink(filepath) == 0) {
        printf("File deleted successfully: %s\n", filepath);
        removeFile(filepath);
        checkFiles();

        return 0;  // Success
    } else {
        perror("Error deleting file");
        return -1;  // Failure
    }
}

void handle_instruction(int client_desc, uint32_t operation, char *client_message) {
    char server_message[2000];

    //printf("%0x\n", operation);

    switch (operation) {
        case 0x0: // LIST
        {
            printf("aici cumva\n");
            char file_list[2000];  // Concatenate file names here
            int indexFile = 0;
            snprintf(server_message, sizeof(server_message), "%x;", SUCCESS);

            int i = 0;
            while (i < file_number)
            {
                int j = 0;
                while (j < strlen(files[i].name))
                {
                    file_list[indexFile] = files[i].name[j];
                    indexFile++;
                    j++;
                }
                file_list[indexFile] = '\0';  // Add '\0' to separate filenames
                indexFile++;
                i++;
            }

            int file_list_length = indexFile;

            char length_str[20];
            snprintf(length_str, sizeof(length_str), "%zu", file_list_length);

            // Append the file_list length as a string to server_message
            strcat(server_message, length_str);
            snprintf(server_message + strlen(server_message), sizeof(server_message) - strlen(server_message), ";");
            int serverLength = strlen(server_message);
            // Copy file_list directly into server_message
            i = 0;
            while (i < file_number)
            {
                int j = 0;
                while (j < strlen(files[i].name))
                {
                    server_message[serverLength] = files[i].name[j];
                    serverLength++;
                    j++;
                }
                server_message[serverLength] = '\0';  // Add '\0' to separate filenames
                serverLength++;
                i++;
            }

            send(client_desc, server_message, serverLength, 0);
        }
        break;

        case 0x1: // DOWNLOAD
        {
            // Assume you have already opened the file and obtained its file descriptor (file_fd).
            printf("Enter case\n");
            printf("client message: %s\n", client_message);
            // int poz = 0;
            // char * op2 = (char*)malloc(4);
            // op2 = semicol_strtok(client_message, &poz);
            // int op = atoi(op2);
            // printf("Code: %x\n", op);

            // char *bytes= (char*)malloc(10);
            // bytes = semicol_strtok(client_message, &poz);
            // int path_bytes = atoi(bytes);

            // char *filepath = (char*) malloc(sizeof(char) * (strlen(client_message) - poz + 1));
            
            // int i=0;
            // while(poz < strlen(client_message))
            // {
            //     filepath[i++] = client_message[poz];
            //     poz++;
            // }
            // filepath[i] = NULL;

            int op= -1;
            int path_bytes = -1;
            char filepath[MAX_PATH_LENGTH] = "";

            // Parse the string
            int fields_parsed = sscanf(client_message, "%x; %d; %[^\n]", &op, &path_bytes, filepath);

            // Check if all fields were successfully parsed
            if (fields_parsed != 3) {
                printf("Error parsing the string.\n");
                // Handle error or set default values
            }

            // sscanf(client_message, "%x; %d; %s", op, path_bytes, filepath);

            printf("Mesaj: %s\n", client_message);
            printf("Code: %x\n", op);
            printf("Nr bytes: %d\n", path_bytes);
            printf("Filepath: %s\n", filepath);

            if(searchForFile(filepath) == -1)
            {
                printf("File not found 0x%x: ", FILE_NOT_FOUND);
            }
            else{
                printf("searching for file\n");

                int file_fd = open(filepath, O_RDONLY);
                // Sending status (0x0 for success).
                struct stat file_stat;
                if (fstat(file_fd, &file_stat) == -1) {
                    perror("Error getting file information");
                    close(file_fd);
                    // Handle the error
                    return;
                }

                // Sending status (0x0 for success).
                uint32_t status = htonl(0x0);
                sprintf(server_message, "%x;", status);

                // Sending the number of response bytes.
                uint32_t response_bytes = htonl(file_stat.st_size);
                sprintf(server_message + strlen(server_message), " %u;", response_bytes);

                printf("Reached here\n");

                // Sending file content using sendfile.
                off_t offset = 0;
                ssize_t sent_bytes = sendfile(client_desc, file_fd, &offset, file_stat.st_size);
                if (sent_bytes == -1) {
                    perror("Error sending file content");
                    close(file_fd);
                    // Handle the error
                    return;
                }

                // Close the file descriptor.
                close(file_fd);
            }
        }
            break;

        case 0x2: // UPLOAD
            {
            // Implement your logic for UPLOAD operation
            // ...
            printf("Sau e aici\n");
            snprintf(server_message, sizeof(server_message), "UPLOAD operation result");
            }
            break;

        case 0x4: //DELETE
            {
            printf("Aici 1\n");
            int op2 = -1;
            int path_bytes2 = -1;
            char filepath2[MAX_PATH_LENGTH] = "";
            
            // Parse the string
            int fields_parsed2 = sscanf(client_message, "%x; %d; %[^\n]", &op2, &path_bytes2, filepath2);
            printf("Aici 1\n");
            if (fields_parsed2 != 3) {
                printf("Error parsing the string.\n");
                // Handle error or set default values
            }

            if(searchForFile(filepath2) > -1)
            {
                printf("File found\n");
                if (deleteFile(filepath2) == 0) {
                    snprintf(server_message, sizeof(server_message), "%x", SUCCESS);
                } else {
                    snprintf(server_message, sizeof(server_message), "%x", OTHER_ERROR);
                }
            }
            else {
                snprintf(server_message, sizeof(server_message), "%x", OTHER_ERROR);
            }
            int serverLength = strlen(server_message);
            send(client_desc, server_message, serverLength, 0);
            }
            break;
        // Add cases for other operations

        case 8:
        {
            printf("Am primit ceva");

            int i = 0;
            while(client_message[i] != ';')
            {
                i++;
            }
            i++; //am trecut de cod operatie => nr bytes srcpath

            char len[10] ="";
            int j = 0;
            while(client_message[i] != ';')
            {
                len[j] = client_message[i];
                j++;
                i++;
            }
            i++;

            uint32_t len_src;
            sprintf(len_src, "%x", len);
            char *src_filepath = (char*) malloc(len_src);
            
            j = 0;
            while(client_message[i] != '\0')
            {
                src_filepath[j] = client_message[i];
                j++;
                i++;
            }
            i++;
            src_filepath[j] = '\0';

            strcpy(len[10], "");
            j = 0;
            while(client_message[i] != ';')
            {
                len[j] = client_message[i];
                i++;
                j++;
            }
            i++;


            uint32_t len_dest;
            sprintf(len_dest, "%x", len);
            char *dest_filepath = (char*) malloc(len_src);
            j = 0;
            while(client_message[i] != '\0')
            {
                dest_filepath[j] = client_message[i];
                j++;
                i++;
            }
            dest_filepath[j] = '\0';

            printf("Src Nr bytes: %x\n", len_src);
            printf("Src Filepath: %s\n", src_filepath);
            printf("Dest Nr bytes: %x\n", len_dest);
            printf("Dest Filepath: %s\n", dest_filepath);

            //moving file using sendfile
            // int src_fd = open(src_filepath, O_RDONLY);
            // if (src_fd == -1) {
            //     perror("Error opening source file");
            //     snprintf(server_message, sizeof(server_message), "%x", OTHER_ERROR);
            //     break;
            // }

            // int dest_fd = open(dest_filepath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            // if (dest_fd == -1) {
            //     perror("Error opening destination file");
            //     close(src_fd);
            //     snprintf(server_message, sizeof(server_message), "%x", OTHER_ERROR);
            //     break;
            // }

            // off_t offset = 0;
            // struct stat src_stat;
            // if (fstat(src_fd, &src_stat) == -1) {
            //     perror("Error getting source file information");
            //     close(src_fd);
            //     close(dest_fd);
            //     snprintf(server_message, sizeof(server_message), "%x", OTHER_ERROR);
            //     break;
            // }

            // // Sending status (0x0 for success).
            // uint32_t status = htonl(0x0);
            // sprintf(server_message, "%x;", status);

            // // Sending the number of response bytes.
            // uint32_t response_bytes = htonl(src_stat.st_size);
            // sprintf(server_message + strlen(server_message), " %u;", response_bytes);

            // // Sending file content using sendfile.
            // ssize_t sent_bytes = sendfile(dest_fd, src_fd, &offset, src_stat.st_size);
            // if (sent_bytes == -1) {
            //     perror("Error moving file content");
            //     close(src_fd);
            //     close(dest_fd);
            //     snprintf(server_message, sizeof(server_message), "%x", OTHER_ERROR);
            //     break;
            // }

            // // Close file descriptors.
            // close(src_fd);
            // close(dest_fd);

            // free(src_filepath);
            // free(dest_filepath);

            // int serverLength2 = strlen(server_message);
            // send(client_desc, server_message, serverLength2, 0);
        }
            break;

        default:
            printf("A intrat aici\n");
            // Handle unknown operation
            snprintf(server_message, sizeof(server_message), "%x", UNKNOWN_OPERATION);
            break;
    }
    
}

void *handle_client(void *socket_descriptor) {
    int client_desc = *((int *)socket_descriptor);
    char client_message[2000];
    memset(client_message, '\0', sizeof(client_message));

    //getting connections
    while (1) {
        // Wait for client message
        ssize_t bytes_received = recv(client_desc, client_message, sizeof(client_message), 0);

        if (bytes_received < 0) {
            perror("Couldn't receive.\n");
            break;
        }

        if (bytes_received == 0) {
            // Connection closed by the client
            printf("Client disconnected.\n");
            break;
        }

        uint32_t operation;
        sscanf(client_message, "%x", &operation);

        printf("Client message: %s\n", client_message);
        printf("Operatie: %x\n", operation);
        //locking connection so it doesn't receive messages while sending out => noise reduction
        pthread_mutex_lock(&mutex);

        handle_instruction(client_desc, operation, client_message);

        //unlocking for further connections;
        pthread_mutex_unlock(&mutex);
        memset(client_message, '\0', sizeof(client_message));
    }

    close(client_desc);
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    // Server set-up

    int socket_desc, client_sock, client_size;
    struct sockaddr_in server_add, client_addr;
    char server_message[2000], client_message[2000];

    memset(server_message, '\0', sizeof(server_message));
    memset(client_message, '\0', sizeof(client_message));

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0) {
        printf("Unable to create socket.\n");
        return -1;
    }

    printf("Socket created successfully.\n");

    server_add.sin_family = AF_INET;
    server_add.sin_port = htons(SERVER_PORT);
    server_add.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (bind(socket_desc, (struct sockaddr *)&server_add, sizeof(server_add)) < 0) {
        printf("Couldn't bind to the port.\n");
        return -1;
    }

    if (listen(socket_desc, CLIENTS) < 0) {
        printf("Error while listening.\n");
        return -1;
    }

    printf("Server is up and listening for connections...\n");

    // I/O multiplexing

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // edge-triggered
    event.data.fd = socket_desc;

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Error creating epoll");
        return -1;
    }

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_desc, &event) == -1) {
        perror("Error adding socket to epoll");
        close(epoll_fd);
        return -1;
    }
    // Client connections

    if (pthread_mutex_init(&mutex, NULL) != 0) {
        printf("Mutex initialization failed.\n");
        return -1;
    }

    // printf("aici\n");

    initializeFiles();
    addAllFiles();

    while (1) {
        printf("Entered while\n");
        struct epoll_event events[CLIENTS];
        int num_events = epoll_wait(epoll_fd, events, CLIENTS, -1);

        for (int i = 0; i < num_events; ++i) {
            if (events[i].data.fd == socket_desc) {
                printf("Entered events\n");
                // Accept new connections
                client_size = sizeof(client_addr);
                client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

                if (client_sock < 0) {
                    perror("Can't accept.\n");
                    continue;
                }

                printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                // Add new client socket to epoll
                struct epoll_event client_event;
                client_event.events = EPOLLIN | EPOLLET;
                client_event.data.fd = client_sock;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_sock, &client_event) == -1) {
                    perror("Error adding client socket to epoll");
                    close(client_sock);
                    continue;
                }
            } else {
                // Handle existing connections
                pthread_t thrd_id;

                if (pthread_create(&thrd_id, NULL, handle_client, &(events[i].data.fd)) < 0) {
                    perror("Couldn't create thread");
                    return -1;
                }

                pthread_detach(thrd_id);
            }
        }
    }

    freeFiles();

    close(epoll_fd);
    close(socket_desc);

    return 0;
}
