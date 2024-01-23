#ifndef GENERAL_USAGE_H
#define GENERAL_USAGE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <dirent.h>

#define SERVER_PORT 90005
#define SERVER_IP "192.168.1.136"
#define CLIENTS 10

// File related constants
#define SERVER_STORAGE "./Server"
#define CLIENT_STORAGE "./Client"
#define MAX_PATH_LENGTH 256
#define MAX_CONTENT_LENGTH 4096

// Constants for operation codes
#define LIST 0x0
#define DOWNLOAD 0x1
#define UPLOAD 0x2
#define DELETE 0x4
#define MOVE 0x8
#define UPDATE 0x10
#define SEARCH 0x20

// Constants for status codes
#define SUCCESS 0x0
#define FILE_NOT_FOUND 0x1
#define PERMISSION_DENIED 0x2
#define OUT_OF_MEMORY 0x4
#define SERVER_BUSY 0x8
#define UNKNOWN_OPERATION 0x10
#define BAD_ARGUMENTS 0x20
#define OTHER_ERROR 0x40

#endif // GENERAL_USAGE_H