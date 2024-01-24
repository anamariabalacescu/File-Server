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
            if(files[i].top10[j]!=NULL)
                free(files[i].top10[j]);
        }
        free(files[i].top10);
        free(files[i].count10);

        for (int j = 0; j < files[i].words_total; j++) {
            if(files[i].words[j])
                free(files[i].words[j]);
        }
        free(files[i].words);
        free(files[i].word_count);

        for (int j = i; j < file_number - 1; j++) {
            files[j] = files[j + 1];
        }

        file_number--;


        return 1; // Success
    } else {
        return 0; // File not found
    }
}

int isFile(const char *path) {
    const char *dot = strrchr(path, '.'); 
    return (dot != NULL && dot[1] != '\0');
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
    //printf("search\n");
    if(file_number < 0)
        return -1; //no files
    
    //printf("%s1\n", filename);

    int i = 0;
    for(int i = 0; i < file_number; i++)
    {
        //printf("search %d\n", i);
        //printf("%d", i);
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

void replace_DIR(char *filepath)
{
    printf("Initial path: %s\n", filepath);
    ///replace ./Server/.....filepath... with ./Client/.....filepath.... => to download
    if (strncmp(filepath, CLIENT_STORAGE, strlen(CLIENT_STORAGE)) == 0)
    {
        memmove(filepath, SERVER_STORAGE, strlen(SERVER_STORAGE));
        memmove(filepath + strlen(SERVER_STORAGE), filepath + strlen(CLIENT_STORAGE), strlen(filepath) - strlen(CLIENT_STORAGE) + 1);
    }
    printf("Modified path: %s\n", filepath);
}

void createDirectories(const char *filePath) {
    char *pathCopy = strdup(filePath);  // Make a copy to avoid modifying the original string
    char *token = strtok(pathCopy, "/");

    char currentPath[1024] = "";  // Assuming a reasonable buffer size

    strcat(currentPath, "./"); 
    while (token != NULL) {
        if(isFile(token))
            break;

        strcat(currentPath, token);

        mkdir(currentPath, 0777);

        strcat(currentPath, "/");
        token = strtok(NULL, "/");
    }

    free(pathCopy);
}

char* insertStringAtIndex(char *fileContent, size_t contentSize, const char *newData, size_t insertionIndex) {
    // Check if the insertion index is within bounds
    if (insertionIndex > contentSize) {
        fprintf(stderr, "Error: Insertion index out of bounds.\n");
        return NULL;
    }

    // Calculate the size of the new content
    size_t newDataSize = strlen(newData);

    // Resize the content buffer to accommodate the new data
    char *modifiedContent = (char *)malloc(contentSize + newDataSize + 1);
    if (modifiedContent == NULL) {
        perror("Error allocating memory");
        return NULL;
    }

    // Copy content before insertion index
    strncpy(modifiedContent, fileContent, insertionIndex);

    // Copy new data to the buffer
    strcpy(modifiedContent + insertionIndex, newData);

    // Copy remaining content after insertion index
    strcpy(modifiedContent + insertionIndex + newDataSize, fileContent + insertionIndex);

    // Null-terminate the modified content
    modifiedContent[contentSize + newDataSize] = '\0';

    // Print or use modifiedContent as needed
    return modifiedContent;

}

int insertDataIntoFile(const char *filename, const char *newData, size_t insertionIndex) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }
    printf("aici2\n");
    // Determine the size of the file
    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    // Read the entire content of the file into a buffer
    char *fileContent = (char *)malloc(fileSize + 1);
    if (fileContent == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return -1;
    }
    fread(fileContent, 1, fileSize, file);
    fileContent[fileSize] = '\0';  // Null-terminate the content

    // Close the file
    fclose(file);
    printf("aici5 %s and %s and %d\n", fileContent, newData, insertionIndex);
    // Insert new data at the specified index in the content
    char* newFileContent= insertStringAtIndex(fileContent, fileSize, newData, insertionIndex);
    if(!newFileContent) return -1;
    // Reopen the file for writing
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        free(fileContent);
        return -1;
    }
    printf("aici7 %s\n", newFileContent);
    // Write the modified content back to the file
    fwrite(newFileContent, 1, strlen(newFileContent), file);
    printf("aici8\n");
    // Close the file
    fclose(file);

}


void handle_instruction(int client_desc, uint32_t operation) {
    char *server_message;

    printf("in handle_instrcution %x\n", operation);

    switch (operation) {
        case LIST:
        {
            uint32_t status = SUCCESS;

            send(client_desc, &status, sizeof(status), 0);

            uint32_t totalLength = 0;
            for (int i = 0; i < file_number; i++) {
                totalLength += strlen(files[i].name) + 1; 
          }

            server_message = (char *)malloc(sizeof(char)*(totalLength));
            
            if (server_message == NULL) {
                fprintf(stderr, "Memory allocation failed for server_message.\n");
                exit(EXIT_FAILURE);
            }

            uint32_t indexMessage = 0;
            for (int i = 0; i < file_number; i++) {
                int filenameLength = strlen(files[i].name);
                memcpy(server_message + indexMessage, files[i].name, filenameLength);
                server_message[indexMessage + filenameLength] = '\0';
                indexMessage += filenameLength + 1;
            }

            send(client_desc, &indexMessage, sizeof(indexMessage), 0);

            for(int i = 0; i < indexMessage; i++)
                if(server_message[i]!='\0')
                    printf("%c", server_message[i]);
                else
                    printf("\n");

            ssize_t bytes_sent = send(client_desc, server_message, indexMessage, 0);

        }
        break;

        case DOWNLOAD: // DOWNLOAD
        {
            printf("Enter case\n");
            
            uint32_t length;
            if(recv(client_desc, &length, sizeof(length), 0) < 0) {
                perror("Error while receiving server's message.\n");
                return -1;
            }

            char *client_message = (char*) malloc (sizeof(char)*length+1);
            
            if(recv(client_desc, client_message, length, 0) < 0) {
                perror("Error while receiving server's message.\n");
                return -1;
            }

            printf("Mesaj: %s and length %d \n", client_message, length);

            printf("avem fisier sau nu: %d\n", searchForFile(client_message));

            if(searchForFile(client_message) == -1)
            {
                printf("File not found 0x%x: ", FILE_NOT_FOUND);
                uint32_t status = FILE_NOT_FOUND;
                send(client_desc, &status, sizeof(status), 0);
                break;
            }
            else{
                uint32_t status = SUCCESS;
                send(client_desc, &status, sizeof(status), 0);
                printf("searching for file\n");

                int file_fd = open(client_message, O_RDONLY);

                status = htonl(0x0);

                unsigned long long file_size;
                struct stat file_status;
                if (fstat(file_fd, &file_status) < 0)
                {
                    perror("Error on getting the file descriptors.\n");
                }
                file_size = file_status.st_size;
                send(client_desc, &file_size, sizeof(file_size), 0);

                printf("size: %d\n", file_size);

                off_t offset = 0;
                ssize_t sent = sendfile(client_desc, file_fd, &offset, file_size);
                if (sent < 0)
                {
                    perror("Failed to send file");
                }

                close(file_fd);
                free(server_message);
            }
        }
            break;

        case UPLOAD: // UPLOAD
            {
            // reverse download
            printf("in upload\n");
            uint32_t length;
            if(recv(client_desc, &length, sizeof(length), 0) < 0) {
                perror("Unable to send message.\n");
                return -1;
            }

            char* path = malloc(length+1);
            if(recv(client_desc, path, length, 0) < 0) {
                perror("Unable to send message.\n");
                return -1;
            }
            path[length] = '\0'; 
            replace_DIR(path);
            printf("Replaced path: %s\n", path);

            // Check if the path contains directories and create them
            createDirectories(path);

            unsigned long long file_size;
            if(recv(client_desc, &file_size, sizeof(file_size), 0) <0) {
                perror("Error while receiving server's message.\n");
                return -1;
            }
            printf("File size: %d\n", file_size);
            char buffer[MAX_CONTENT_LENGTH];
            FILE *file = fopen(path, "w");
            if (file == NULL)
            {
                perror("Error opening file");
                exit(1);
            }
            while (file_size > 0) {
                memset(buffer, 0, MAX_CONTENT_LENGTH);
                ssize_t bytes = 0;
                if (file_size < MAX_CONTENT_LENGTH) {
                    bytes = recv(client_desc, buffer, file_size, 0);
                } else {
                    bytes = recv(client_desc, buffer, MAX_CONTENT_LENGTH, 0);
                }

                if (bytes < 0) {
                    uint32_t status_code = OTHER_ERROR;
                    fclose(file);
                    send(client_desc, &status_code, sizeof(status_code), 0);
                    break;
                }
                printf("File size: %d, buffer: %s, bytes: %d\n", file_size, buffer, bytes);
            
                fwrite(buffer, 1, bytes, file);
                file_size = file_size - bytes;
            }
            fclose(file);
            addFile(path);
            checkFiles();

            uint32_t status_code = SUCCESS;
            send(client_desc, &status_code, sizeof(status_code), 0);
            }
            break;

        case DELETE: //DELETE
            {
                printf("caz DELETE\n");
                uint32_t pathlength;
                if(recv(client_desc, &pathlength, sizeof(pathlength), 0) < 0)
                {
                    perror("Error reciving client's message\n");
                    return -1;
                }
                //got path length
                char *client_message = (char*) malloc(pathlength+1);

                if(recv(client_desc, client_message, pathlength, 0) < 0){
                    perror("Error reciving clien't message\n");
                    return -1;
                }
                client_message[pathlength] = '\0';

                printf("Length: %d\n", pathlength);
                printf("Path: %s\n", client_message);

                uint32_t op_status;
                if(searchForFile(client_message) > -1)
                {
                    printf("File found\n");
                    if (deleteFile(client_message) == 0) {
                        op_status = SUCCESS;
                    } else {
                        op_status = OTHER_ERROR;
                    }
                }
                else {
                    op_status = OTHER_ERROR;
                }
                send(client_desc, &op_status, sizeof(op_status), 0);

                printf("status sent\n");
                }
                break;
        case MOVE:
        {
            printf("CASE MOVE\n");
            //get files lengths and paths
            uint32_t src_len;
            if(recv(client_desc, &src_len, sizeof(src_len), 0) < 0)
            {
                perror("Error getting the source file length\n");
                return -1;
            }
            
            char *src_path = malloc(src_len + 1);
            if(recv(client_desc, src_path, src_len, 0) < 0)
            {
                perror("Error getting the source file length\n");
                return -1;
            }
            src_path[src_len] = '\0';
            printf("Source length: %d\n", src_len);
            printf("Source file: %s\n", src_path);
            
            uint32_t dest_len;
            if(recv(client_desc, &dest_len, sizeof(dest_len), 0) < 0)
            {
                perror("Error getting the source file length\n");
                return -1;
            }
            
            char *dest_path = malloc(dest_len + 1);
            if(recv(client_desc, dest_path, dest_len, 0) < 0)
            {
                perror("Error getting the source file length\n");
                return -1;
            }
            dest_path[dest_len] = '\0status_code';
            
            //got the files but gotta check them
            printf("Destination length: %d\n", dest_len);
            printf("Destination file: %s\n", dest_path);

            if(searchForFile(src_path) == -1 || searchForFile(dest_path) != -1)
            {
                uint32_t status = BAD_ARGUMENTS;
                send(client_desc, &status, sizeof(status), 0);
            }
            else{
                printf("aici dupa search\n");
                createDirectories(dest_path);
                int src_fd = open(src_path, O_RDONLY);
                int dest_fd = open(dest_path, O_RDWR | O_CREAT, 0666);

                unsigned long long file_size;
                struct stat file_status;
                if (fstat(src_fd, &file_status) < 0)
                {
                    perror("Error on getting the file descriptors.\n");
                }
                file_size = file_status.st_size;

                sendfile(dest_fd, src_fd, 0, file_size);

                close(dest_fd);
                close(src_fd);
                deleteFile(src_path);
                addFile(dest_path);
                uint32_t status = SUCCESS;
                send(client_desc, &status, sizeof(status), 0);
            }
        }
        break;
        case 10:
        {
            printf("MOVE CASE\n");

            uint32_t len = 0;
            recv(client_desc, &len, sizeof(len), 0);
            printf("len path= %d\n", len);

            char* path = malloc(len + 1);

            recv(client_desc, path, len, 0);
            path[len] = '\0';

            printf("path = %s\n", path);


            uint32_t st_byte = 0;
            recv(client_desc, &st_byte, sizeof(st_byte), 0);
            printf("Byte = %d\n", st_byte);


            uint32_t cont_len = 0;
            recv(client_desc, &cont_len, sizeof(cont_len), 0);
            
            printf("len of cont= %d", cont_len);

            char *content = malloc(cont_len + 1);
            recv(client_desc, content, cont_len, 0);
            content[cont_len] = '\0'; 
            printf("content = %s\n", content);

            // uint32_t len;
            // recv(client_desc, &len, sizeof(len), 0);
            // char *path= malloc(len + 1);
            // recv(client_desc, path, len, 0);
            // printf("path %s",path);
            // path[len] = '\0';

            // printf("inainte de search\n");
            // printf("Search result %d\n", searchForFile(path));
            // if(searchForFile(path) == -1) {
            //     printf("File not found\n");
            //     uint32_t status_code;
            //     status_code = OTHER_ERROR;
            //     send(client_desc, &status_code, sizeof(status_code), 0);
            //     break; //not really great, it still sends stuff
            // }


            // printf("dupa search\n");
            // uint32_t start_byte;
            // recv(client_desc, &start_byte, sizeof(start_byte), 0);
            
            // printf("Byte = %d\n", start_byte);
            
            // uint32_t buffer_size;
            // recv(client_desc, &buffer_size, sizeof(buffer_size), 0);
            // printf("buffer = %d\n", buffer_size);
            // char *new_sequence = malloc(buffer_size+1);
            // recv(client_desc, new_sequence, buffer_size, 0);
            // printf("len of new = %d", strlen(new_sequence));
            // ssize_t totalSize = 0;
            // int ok = 0;
            // // while (ok == 0) {
            // //     printf("ok = %d\n", ok);
            // //     recv(client_desc, &buffer_size, sizeof(buffer_size), 0);

            // //     char * aux = malloc(buffer_size + 1);

            // //     printf("got size %d\n", buffer_size);

            // //     if(buffer_size < MAX_CONTENT_LENGTH)
            // //         ok = 1;

            // //     recv(client_desc, aux, buffer_size, 0);
            // //     printf("aux = %s\n", aux);
            // //     printf("am ceva\n");

            // //     size_t newSize = totalSize + buffer_size;
            // //     new_sequence = realloc(new_sequence, newSize + 1);  // +1 for null terminator

            // //     if (new_sequence == NULL) {
            // //         perror("Error reallocating memory");
            // //         free(aux);
            // //         exit(EXIT_FAILURE);
            // //     }

            // //     // Copy data from aux to new_sequence
            // //     memcpy(new_sequence + totalSize, aux, buffer_size);
            // //     printf("newseq aici = %s\n", new_sequence);
            // //     // Update the total size
            // //     totalSize = newSize;
            // //     free(aux);
            // // }

            // // recv(client_desc, &buffer_size, sizeof(buffer_size), 0);
            // // printf("buffer size %d\n", buffer_size);
            // // char *new_sequence = malloc(buffer_size+1);
            // // recv(client_desc, new_sequence, buffer_size+1, 0);

            // uint32_t status_code;
            // printf("%s ee\n", new_sequence);
            // // Insert the new_sequence into the file
            if(insertDataIntoFile(path, content, st_byte) == -1) {
                uint32_t status_code = OTHER_ERROR;
                send(client_desc, &status_code, sizeof(status_code), 0);
                break;
            }
            printf("aici9\n");
            uint32_t status_code = SUCCESS;
            send(client_desc, &status_code, sizeof(status_code), 0);
            printf("aici10\n");
        }
        break;
        default:
            printf("A intrat aici\n");
            // Handle unknown operation
            snprintf(server_message, sizeof(server_message), "%x", UNKNOWN_OPERATION);
            break;

        printf("out of case\n");
    }
    
}

void *handle_client(void *socket_descriptor) {
    printf("in handle_client \n");
    int client_desc = *((int *)socket_descriptor);
    uint32_t byteSize = 0;

    //getting connections
    while (1) {
        // Wait for client message
        
        uint32_t operation;
        pthread_mutex_lock(&mutex);
        if(recv(client_desc, &operation, sizeof(operation), 0) < 0) {
            perror("Error while receiving server's message.\n");
            return -1;
        }

        printf("Operatie: %u\n", operation);
        //locking connection so it doesn't receive messages while sending out => noise reduction


        handle_instruction(client_desc, operation);//, client_message);

        //unlocking for further connections;
        pthread_mutex_unlock(&mutex);
        //memset(client_message, '\0', sizeof(client_message));
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
        //printf("Entered while\n");
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
