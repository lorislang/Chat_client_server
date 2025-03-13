#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#define MAX_SIG 1024
#define MAX_CLIENTS 32
int tcp_socket;
struct sockaddr_in addr;
pthread_t tid_listener;


struct client_descrpt { //client struct
    pthread_t tid;
    char user[20];
    int socket;
};  

struct client_descrpt clients[MAX_CLIENTS];

int connected = 0; // connected clients
int nclients = 0; // total number of clients

void* client_thread(void* args) {
    int clientid = *(int *) args;  // retrieve the client id from args
    int socket = clients[clientid].socket; // use the socket stored on client id position
    char message[MAX_SIG];
    char user[MAX_SIG];
    int nchars;
    nchars = read( socket , user, MAX_SIG); // read the user first
    strcpy(clients[clientid].user, user);   
    printf("%s connected on client %d\n" , user, clientid);
    while ((nchars = recv( socket , message, MAX_SIG,0)) > 0 ) { 
        message[nchars] = 0; // make sure the string is null terminated
        
        // check if shutdown or quit was invoked
        if (strncmp(message,"/shutdown",9) == 0 || strncmp(message,"/quit",5) == 0) {
            close(socket);
            clients[clientid].socket = -1; // signal that the connection is closed
            connected--;
            printf("%s disconnected\n", user);
            if (strncmp(message,"/shutdown",9) == 0) {
                pthread_cancel(tid_listener); // cancel the listener thread. no more connections allowed
            }
            return NULL;
        }
        // print the chat message to the server console
        printf("%s : %s",user, message);
        if (strncmp(message,"/w",2) == 0 ) {
            char* cmd = strtok (message, " ");
            char* userw = strtok (NULL, " ");
            char* msg = strtok (NULL, "\0");
            
            for (int i = 0; i < nclients; i++) {

                if (strcmp (userw, clients[i].user) == 0) {
                    send(clients[i].socket , user, strlen(user), 0);
                    send(clients[i].socket , " (whispers) ", 12, 0);
                    send(clients[i].socket , msg , strlen(msg) , 0 ); 
                }

            }
            printf("%s (whispers) %s \n", userw, msg);
        }
        else {
            for (int i = 0; i < nclients; i++) {
                // send to all other clients which are still connected
                if (i != clientid && clients[i].socket > -1) {
                    send(clients[i].socket , user, strlen(user), 0);
                    send(clients[i].socket , " : ", 3, 0);
                    send(clients[i].socket , message , strlen(message) , 0 );
                }
            }
        }
    }
}

void* listener() {

    // listen for incoming connections
    if (listen(tcp_socket, MAX_CLIENTS) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    int clientid; // to store the current client id while creating the client thread
    while (1) {
        // only add more clients if there is enough space in the corresponding arrays
        if (nclients < MAX_CLIENTS) {
            // accept incoming connections
            int addrlen = sizeof(addr);
            if ((clients[nclients].socket = accept(tcp_socket, (struct sockaddr *)&addr,(socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            clientid = nclients; // temp storage for the client id. it would get overwritten otherwise
                                 // alternatively use a mutex to make sure the value is protected during the thread creation     
            pthread_create(&clients[nclients].tid, NULL, client_thread, &clientid); // pass the client id as parameter
            nclients++; // increase the number of clients 
            connected++;  // and the number of connected clients
        }
    }

} 

int main (int argc, char** argv) {

    char message[MAX_SIG];    
    int port;
    port = atoi(argv[1]); // get the port from the 1 argument provided
    // set the parameters for addr 
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY); // on server

    // create and bind the listener socket
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(bind(tcp_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("bind failed\n");
        return EXIT_FAILURE;
    }
    // create the listener thread. it will accept incoming connections and create the client threads
    pthread_create(&tid_listener, NULL, listener, NULL);
    // wait for the listener thread to finish. this happens through a call to thread_cancel in a client thread
    pthread_join(tid_listener, NULL);

    // wait for the remaining clients to disconnect
    printf("Server is shutting down. Waiting for %d clients to disconnect\n", connected);
    for (int i = 0; i < nclients; i++) {
        pthread_join(clients[i].tid, NULL);
    }
    return EXIT_SUCCESS;

}