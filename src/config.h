#ifndef MSG_SIZE
#define MSG_SIZE 2000
#endif

#define CHUNK_SIZE 4096
/*
// server address and port number
#define PORT 8080
#define SERVER_IP "192.168.45.67"
*/

// local address and port number
#define PORT 5000
#define SERVER_IP "127.0.0.1"

#define SERVER_BACKLOG 100


#define LITERAL_DELIM ','
#define DELIM ","

#ifdef _WIN32
    #define PATH_DELIMITER "\\"
#else
    #define PATH_DELIMITER "/"
#endif

#ifdef _WIN32
    #define LITERAL_PATH_DELIMITER '\\'
#else
    #define LITERAL_PATH_DELIMITER '/'
#endif

#define DEFAULT_CLIENT_DIR "client_data/"
#define DEFAULT_CLIENT_FILENAME "file.txt"
#define DEFAULT_SERVER_DIR "server_data/"
#define DEFAULT_SERVER_FILENAME "file.txt"
#define DEFAULT_FILE_EXT "txt"