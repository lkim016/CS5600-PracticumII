/**
 * @file server.c / source file for TCP Socket Server.
 * @authors Lori Kim / CS5600 / Northeastern University
 * 
 * @date Dec 5, 2025 / Fall 2025
 * adapted from: 
 *   https://www.educative.io/answers/how-to-implement-tcp-sockets-in-c
 */


#include "server_utils.h"

// need to declare this here otherwise experienced error
pthread_mutex_t stop_mutex = PTHREAD_MUTEX_INITIALIZER;
bool stop_server = false;

/**
 * @brief handles the CLI args commands that are pased to it as a message by the client
 *
 * @return int - 0 if success otherwise -1 for error 
 */
int main(void) {
  int socket_desc, client_sock;
  socklen_t client_size;
  struct sockaddr_in server_addr, client_addr;
  
  // Create socket:
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  
  if(socket_desc < 0) {
    printf("Error while creating socket\n");
    close(socket_desc);
    return -1;
  }
  printf("Socket created successfully\n");
  
  // Set port and IP:
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
  
  // Bind to the set port and IP:
  if(bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr))<0) {
    printf("Couldn't bind to the port\n");
    close(socket_desc);
    return -1;
  }
  printf("Done with binding\n");
  
  // Listen for clients:
  if(listen(socket_desc, 1) < 0) {
    printf("Error while listening\n");
    close(socket_desc);
    return -1;
  }
  
  while(1) { // loop through to have server continue listening until shut down
    pthread_mutex_lock(&stop_mutex);
    bool should_stop = stop_server;
    pthread_mutex_unlock(&stop_mutex);

    if (should_stop) {
        break;  // Exit the thread if server is stopping
    }
    
    printf("\nListening for incoming connections on port %d\n", PORT);
    
    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr*)&client_addr, &client_size);
    
    if (client_sock < 0){
      printf("Can't accept\n");
      continue;
    }
    
    // create a new socket metadata for every client connection
    socket_md_t* server_metadata = create_socket_md(client_sock);
    if (!server_metadata) {
      printf("Failed to create socket metadata\n");
      continue;
    }

    // set_server_sock_fd(server_metadata, socket_desc);
    printf("SERVER CONNECTION---\n");
    printf("Client connected at IP: %s and port: %i\n", 
          inet_ntoa(client_addr.sin_addr), 
          ntohs(client_addr.sin_port));

    rcv_request(server_metadata);

    server_cmd_handler(server_metadata);
    /*
    // Create reader and writer threads:
    pthread_t thread;

    // cmd hanling thread
    if (pthread_create(&thread, NULL, server_cmd_handler, (void*)server_metadata) != 0) {
        perror("Failed to create reader thread\n");
        close(server_metadata->client_sock_fd);
        continue;
    }

    pthread_detach(thread); // Detach the thread to manage its own cleanup
    */
  }

  close(socket_desc);
    
  return 0;
}
