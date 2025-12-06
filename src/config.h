#ifndef MSG_SIZE
#define MSG_SIZE 2000
#endif

#define CHUNK_SIZE 4096

// port number
#define PORT 8080
// server address
#define SERVER_IP "192.168.45.67"

#define DELIMITER ","

#ifdef _WIN32
    #define PATH_DELIMITER '\\'
#else
    #define PATH_DELIMITER '/'
#endif

// #define DEFAULT_CLIENT_PATH "data/"
#define DEFAULT_CLIENT_FILENAME "file.txt"
#define DEFAULT_SERVER_PATH "server_data/"
#define DEFAULT_SERVER_FILENAME "file.txt"
#define DEFAULT_FILE_EXT ".txt"