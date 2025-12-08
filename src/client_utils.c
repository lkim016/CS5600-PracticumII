
/**
 * @file client_utils.c / source file for Client TCP Socket utilitis.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 */

#include "client_utils.h"


/*
client_cmd_handler
*/
void client_cmd_handler(socket_md_t* sock) {
  if (!sock) {
      fprintf(stderr, "ERROR: Socket is NULL\n");
      return;
  }
  
  // Clean buffers:
  char server_message[MSG_SIZE]; // Declare server message - since its a stream will send as comma-delimited string
  memset(server_message,'\0',sizeof(server_message));
  // const char* msg = NULL;
  // handle different commands
  switch (sock->command) {
    case WRITE:
      int sock_fd =sock->client_sock_fd;
      const char* filepath = sock->first_filepath;
      // check if file_exits - if yes then send and receive
      long file_size = get_file_size(filepath);
      sock->file_size = htonl(file_size);
      printf("Client: File exists with size of %ld.\n", file_size);
      printf("Long File Size: %ld\n", file_size);
      if (file_exists(filepath) == 1 && file_size > 0) {
          int request_sent_status = send_request(sock);
          if (request_sent_status == 0) {
              printf("Sending file data (%lu bytes)...\n", file_size);
              if (send_file(filepath, sock_fd) < 0) {
                  fprintf(stderr, "send_file failed\n");
                  return;
              }
              printf("File sent..\n");
          }
      } else {
          fprintf(stderr, "Client: File does not exist.\n");
      }
      
      break;
    case GET:
        int sock_fd = sock->client_sock_fd;
        const char* filepath = sock->sec_filepath;

        pthread_mutex_lock(&file_mutex);  // Lock filesystem
        int folder_exists = folder_not_exists_make(filepath);
        if (folder_exists == 0) {
            printf("Path was created\n");

        }
        pthread_mutex_unlock(&file_mutex);  // Lock filesystem

        uint32_t size = sock->file_size;
        char* sec_filepath = sock->sec_filepath;
        printf("Receiving file (%u bytes) to: %s\n", size, sec_filepath);
        ssize_t file_rcvd_bytes = rcv_file(sock_fd, sec_filepath, size);

        if (folder_exists < 0) {
            if (file_rcvd_bytes < 0 ) {
                msg = build_send_msg(threadID, "Server: Error receiving file", "");
            }
            msg = build_send_msg(threadID, "Server: Error folder was not able to be made", "");
        } else {
            msg = build_send_msg(threadID,"Server: File sent successfully!", "");
        }
        

        // if (send_msg(sock_fd, msg) < 0) {
        //     perror("Failed to send response to client\n");
        // }

        printf("%s\n", msg);

        if (msg != NULL) {
          free(msg);
        }
        break;
    case RM:
        // Wait for acknowledgment from the other socket before declaring success
        ssize_t rm_recv = recv(sock->client_sock_fd, server_message, sizeof(server_message), 0);
        if (rm_recv < 0) {
            perror("Error receiving acknowledgment from server");
            return;
        } else if (rm_recv == 0) {
            printf("Server closed connection\n");
            return;
        }

        print_server_resp(server_message);

      break;
    case STOP:
      // Receive the server's response:
      if(recv(sock->client_sock_fd, server_message, sizeof(server_message), 0) < 0) {
        printf("Error while receiving server's msg\n");
        return;
      }
      
      printf("Server's response:\n%s\n",server_message);
      break;
    default:
      printf("Unknown command\n");
      break;
    }
}


/* set_client_sock_metadata */
void set_client_sock_metadata(socket_md_t* sock, int argc, char* argv[]) {
  // set members of socket object
  set_command(sock, str_to_cmd_enum(argv[1]));

  if (argc > 2) { // set first_filepath
      set_first_fileInfo(argv[2], sock);
      set_first_filepath(sock);
  }
  
  if (argc > 3) { // WRITE - if argv[3] is null then use file name of arfv[2] / GET - if argv[3] is null then need to use default local path
      set_sec_fileInfo(argv[3], sock);
      set_sec_filepath(sock);
  }
}



/*
build_message
NOTE: free() ptr after use
*/
char* build_message(int argc, char* argv[]) {
   //-------------- construct client message
    int msize = 0;
    // Calculate message length when combined by delimiters - command,filename,file_size,remote_filename
    for(int i = 1; i < argc; i++) {
      msize += strlen(argv[i]) + 1; // Adding 1 for the delimiter/comma
    }

    char* server_message = (char*)malloc(msize + 1); // +1 for null terminator
    if (!server_message) {
        perror("malloc failed for server_message");
        return NULL;
    }
    server_message[0] = '\0'; // initialize malloced string

    // Construct the message with delimiters
    // example: ./rfs WRITE data/file.txt remote/file.txt
    // becomes: WRITE, data/file.txt, remote/file.txt
    int msgi = 0;
    for(int i = 1; i < argc; i++) {
      char* command = argv[i];
      int c = 0;
      while(command[c] != '\0') {
        server_message[msgi++] = command[c++];
      }
      server_message[msgi++] = LITERAL_DELIM;
    }

    printf("Client Message: %s\n", server_message); // DEBUG:
    return server_message;
}


/*
send_args_message
*/
ssize_t send_args_message(socket_md_t* sock, char* message) {

    // send client message - this sends the commands
    ssize_t sent_size = send_msg(sock->client_sock_fd, message);
    free(message);

    if (sent_size > 0) {
      printf("Message of size %ld successfully sent\n", sent_size);
      return sent_size;
    } else {
      perror("Failed to send message");
      return -1;
    }
}

/*
__print_server_resp
*/
void __print_server_resp(const char* response) {
  printf("Client:\n Server Response: %s\n", response);
}