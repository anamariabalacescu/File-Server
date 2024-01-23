#include "general_usage.h"


typedef struct file_data{
    char *name;
    int hasContent;
}file_data;

file_data *files = NULL;
int file_number = 0;

void initializeFiles()
{
    files = (file_data*) malloc(sizeof(file_data) * 10);
    for(int i = 0; i < 10; i++)
    {
        files[i].name = NULL;
        files[i].hasContent = 0;
    }
}

void freeFiles()
{
    for(int i = 0; i < file_number; i++)
    {
        free(files[i].name);
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

    listFilesRecursively(CLIENT_STORAGE, files, &index);
    file_number = index;
    // Print or process the file_data structures as needed
    for (int i = 0; i < index; i++) {
        printf("File Name: %s\n", files[i].name);
    }
}

int searchForFile(char *filename)
{
    if(file_number < 0)
        return -1; //no files
    
    int i = 0;
    for(int i = 0; i < file_number; i++)
    {
        if(strstr(files[i].name, filename)!=NULL)
            return i;
    }
    return -1;
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
    int i = 0;
    // Parse the string with \0
    int numberOfFiles=0;
    //first we take number of bytes 
    char numberOfBytes[200];
    int lenNumberOfBytes=0;
    int totalLengthMessage=0;
    int index=0;
    
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

void parseString(const char *input, char ***result, size_t *numTokens) {
    size_t length = strlen(input);
    const char *start = input;
    const char *end = input;

    // Count the number of tokens
    *numTokens = 0;
    while (end < input + length) {
        end = (char *)memchr(start, '\0', input + length - start);
        
        if (end == NULL) {
            // No more null characters found
            end = input + length;
        }

        (*numTokens)++;
        start = end + 1;
    }

    // Allocate memory for the array of pointers
    *result = (char **)malloc(*numTokens * sizeof(char*));

    // Reset start pointer
    start = input;

    // Extract tokens and store pointers in the result array
    for (size_t i = 0; i < *numTokens; i++) {
        end = (char *)memchr(start, '\0', input + length - start);
        
        if (end == NULL) {
            // No more null characters found
            end = input + length;
        }

        size_t tokenLength = end - start;
        (*result)[i] = (char *)malloc((tokenLength + 1) * sizeof(char));
        memcpy((*result)[i], start, tokenLength);
        (*result)[i][tokenLength] = '\0';

        // Move to the next character after the null character
        start = end + 1;
    }
}

void replace_DIR(char *filepath)
{
    ///replace ./Server/.....filepath... with ./Client/.....filepath.... => to download
    if (strncmp(filepath, SERVER_STORAGE, strlen(SERVER_STORAGE)) == 0)
    {
        memmove(filepath, CLIENT_STORAGE, strlen(CLIENT_STORAGE));
        memmove(filepath + strlen(CLIENT_STORAGE), filepath + strlen(SERVER_STORAGE), strlen(filepath) - strlen(SERVER_STORAGE) + 1);
    }
}

void createDirectories(char *filepath) {
    char *sep = strrchr(filepath, '/');
    if (sep != NULL) {
        *sep = '\0'; // temporarily truncate the path
        if (mkdir(filepath, 0777) == -1 && errno != EEXIST) {
            perror("Error creating directories");
            exit(EXIT_FAILURE);
        }
        *sep = '/'; // restore the path
        chmod(filepath, 0777);
    }
    free(sep);
}


void handle_option(int socket_descriptor)
{
    printf("Enter option: ");

    uint32_t op;
    
    if (scanf("%u" , &op) != 1) {
        printf("Invalid input. Please enter a valid uint32_t value.\n");
        return 1; // Return an error code
    }
    printf("Operatia %u\n", op);
    // char client_message[2000], server_message[2000];
    char *client_message, *server_message;
    uint32_t status_code;

    if(send(socket_descriptor, &op, sizeof(op), 0) < 0) {
        perror("Unable to send message.\n");
        return -1;
    }

    switch (op)
    {
        case 0x0:
            {
                printf("Treating op_Code...\n");
                //Send operation

                //Receive status code 
                if(recv(socket_descriptor, &status_code, sizeof(status_code), 0) < 0) {
                    perror("Error while receiving server's message.\n");
                    return -1;
                }

                if(status_code == 0)
                {
                    printf("Status 0\n");
                    //trimitem 0x0 si luam inapoi fisierele
                    uint32_t serverlength = 0;
                    if(recv(socket_descriptor, &serverlength, sizeof(serverlength), 0) < 0) {
                        perror("Error while receiving server's message.\n");
                        return -1;
                    }

                    //printf("%u = length\n", serverlength);
                    server_message = (char*)malloc(sizeof(char)*(serverlength));
                    //printf("%d", strlen(server_message));
                    if(recv(socket_descriptor, server_message, serverlength, 0) < 0) {
                        perror("Error while receiving server's message.\n");
                        return -1;
                    }
                    printf("message: %s\n", server_message);
                    //Parse filenames  
                    // char **result;
                    // size_t numFiles;
                    // parseString(server_message, &result, &numFiles);

                    // // Print the files and free afterwards
                    // for (size_t i = 0; i < numFiles; i++) {
                    //     printf("%zu) %s\n", i + 1, result[i]);
                    //     free(result[i]);
                    // }

                    for(int i = 0; i < serverlength; i++)
                    {
                        if(server_message[i]=='\0')
                            printf("\n");
                        else 
                            printf("%c", server_message[i]);
                    }
                    
                    free(server_message);
                }

                //finally working
            }
            break;

        case 0x1:
        {
            char path[MAX_PATH_LENGTH];
            printf("Select filepath to download: ");
            scanf("%s", &path);
            uint32_t length = strlen(path);
            if(send(socket_descriptor, &length, sizeof(length), 0) < 0) {
                perror("Unable to send message.\n");
                return -1;
            }

            if(send(socket_descriptor, path, length, 0) < 0) {
                perror("Unable to send message.\n");
                return -1;
            }

            if(recv(socket_descriptor, &status_code, sizeof(status_code), 0) < 0) {
                perror("Error while receiving server's message.\n");
                return -1;
            }

            printf("%u\n", status_code);

            replace_DIR(path);
            printf("Replaced path: %s\n", path);
            
            if (status_code == FILE_NOT_FOUND) {
                printf("File not found on the server.\n");
                break;
            }

            // Check if the path contains directories and create them
            createDirectories(path);

            unsigned long long file_size;
            recv(socket_descriptor, &file_size, sizeof(file_size), 0);
            char buffer[MAX_CONTENT_LENGTH];
            FILE *file = fopen(path, "w");
            if (file == NULL)
            {
                perror("Error opening file");
                exit(1);
            }
            while (file_size > 0)
            {
                memset(buffer, 0, MAX_CONTENT_LENGTH + 1);
                ssize_t bytes = 0;
                if(file_size < MAX_CONTENT_LENGTH)
                    bytes = recv(socket_descriptor, buffer, file_size, 0);
                else
                    bytes = recv(socket_descriptor, buffer, MAX_CONTENT_LENGTH, 0);

                if (bytes < 0)
                {
                    break;
                }
                fwrite(buffer, 1, bytes, file);
                //printf("%s", buffer);
                file_size = file_size - bytes;
            }
            fclose(file);
            addFile(path);
            checkFiles();
            // printf("Directories/Files created\n");
            
            // // Open the file in write and create mode --needs to be created if it doesn't exist(probably doesn't)
            // int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0666);
            // if (fd == -1) {
            //     perror("Error opening/creating file");
            //     exit(EXIT_FAILURE);
            // }

            // printf("File opened for writting\n");

            // int dim = 0;
            // recv(socket_descriptor, &dim, sizeof(dim), 0);
            // // Receive and write file content
            // char *buffer = malloc(dim);
            // memset(buffer, 0, dim); // Initialize the buffer with zeros

            // ssize_t total_received_bytes = 0;
            // ssize_t received_bytes;

            // while (total_received_bytes < dim) {
            //     received_bytes = recv(socket_descriptor, buffer + total_received_bytes, dim - total_received_bytes, 0);

            //     if (received_bytes <= 0) {
            //         perror("Error receiving file content");
            //         break;
            //     }

            //     total_received_bytes += received_bytes;
            // }

            // printf("received all\n");
            // for(int i =0 ; i < total_received_bytes; i++)
            //     printf("%c", buffer[i]);

            // // if (total_received_bytes == dim) {
            // if(write(fd, buffer, total_received_bytes) == -1)
            // {
            //     perror("Incomplete file content\n");
            // }
            // // } else {
            // //     perror("Incomplete file content received");
            // // }

            // // Close the file
            // close(fd);

            printf("File downloaded successfully.\n");
        }
            break;

        case 0x2: //upload
        {
            //reverse download basically
            printf("Enter UPLOAD case\n");

            int ok = 0;
            //printf("client message: %s\n", client_message);
            do{
                printf("Select the client's file you want to upload: ");
                char path[MAX_PATH_LENGTH];
                scanf("%s", path);

                printf("Fisier: %s\n", path);

                printf("avem fisier sau nu: %d\n", searchForFile(path));

                if(searchForFile(path) == -1)
                {
                    printf("File not found 0x%x.\nPlease check the path specified and retype.\n", FILE_NOT_FOUND);
                }
                else{
                    ok = 1;
                    uint32_t length = strlen(path);
                    if(send(socket_descriptor, &length, sizeof(length), 0) < 0) {
                        perror("Error while receiving server's message.\n");
                        return -1;
                    }//send length

                    if(send(socket_descriptor, path, length, 0) < 0) {
                        perror("Error while receiving server's message.\n");
                        return -1;
                    }//send path

                    printf("sending file\n");

                    int file_fd = open(path, O_RDONLY);

                    unsigned long long file_size;
                    struct stat statfile;
                    if (fstat(file_fd, &statfile) < 0)
                    {
                        perror("Error on fd");
                    }
                    file_size = statfile.st_size;

                    printf("File size: %d\n", file_size);
                    printf("Path again: %s\n", path);

                    send(socket_descriptor, &file_size, sizeof(file_size), 0); //send size of content

                    printf("size: %d\n", file_size);

                    off_t offset = 0;
                    ssize_t sent = sendfile(socket_descriptor, file_fd, &offset, file_size); //send content
                    if (sent < 0)
                    {
                        perror("Failed to send file");
                    }
                    close(file_fd);
                }
            }while(ok ==0);
            if(recv(socket_descriptor, &status_code, sizeof(status_code), 0) < 0)
            {
                perror("Error in getting the status");
                exit -1;
            }
            printf("Status %d\n", status_code);
        }
            break;

        case 4:
        {
            //delete
            printf("in delete\n");
            char path[MAX_PATH_LENGTH];
            printf("Select filepath to delete: ");
            scanf("%s", &path);
            printf("Path: %s", path);
            uint32_t length = strlen(path);
            printf("Length: %u", length);
            if(send(socket_descriptor, &length, sizeof(length), 0) < 0) {
                perror("Unable to send message.\n");
                return -1;
            } //send length of file to delete

            if(send(socket_descriptor, path, length, 0) < 0) {
                perror("Unable to send message.\n");
                return -1;
            } //send path of file to delete

            //receive status of operation
            uint32_t op_status;
            if(recv(socket_descriptor, &op_status, sizeof(op_status), 0) < 0) {
                perror("Error while receiving server's message.\n");
                return -1;
            }

            printf("Status %d", op_status);

        }
            break;

        case MOVE: //MOVE
        {
            //upload+delete
            int ok = 0;
            
            printf("MOVE CASE\n");
            printf("Fisier sursa de pe SERVER: ");
            char srcpath[MAX_PATH_LENGTH];
            scanf("%s", srcpath);
            printf("Fisier destinatie de pe SERVER: ");
            char destpath[MAX_PATH_LENGTH];
            scanf("%s", destpath);

            printf("Verify src path: %s\n", srcpath);
            printf("Verify dest path: %s\n", destpath);

            uint32_t src_len = strlen(srcpath);
            srcpath[src_len] = '\0';
            uint32_t dest_len = strlen(destpath);
            destpath[dest_len] = '\0';
            printf("Len src: %d\n", src_len);
            printf("Len dest: %d\n", dest_len);
            
            send(socket_descriptor, &src_len, sizeof(src_len), 0);
            if(send(socket_descriptor, &srcpath, src_len, 0) < 0)
            {
                perror("Error in sending source file.\n");
                return -1;
            }

            send(socket_descriptor, &dest_len, sizeof(dest_len), 0);
            if(send(socket_descriptor, &destpath, dest_len, 0) < 0)
            {
                perror("Error in sending source file.\n");
                return -1;
            }

            if(recv(socket_descriptor, &status_code, sizeof(status_code), 0) < 0)
            {
                perror("Error receiving status.\n");
                return -1;
            }
            printf("Status: %u\n", status_code);
        }    
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

    initializeFiles();
    addAllFiles();
    
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
        // printf("Option: %x\n",option);
        handle_option(socket_descriptor);
    }

    close(socket_descriptor);
    
    return 0;
}

