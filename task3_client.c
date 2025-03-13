#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#define MAX_SIG 1024
int tcp_socket;

void* receiver() {
    // receiver thread reads from the same socket which is used for sending messages to the server
    char message[MAX_SIG];
    int valread;
    while ((valread = recv( tcp_socket , message, MAX_SIG, 0 )) > 0 ) {
        message[valread] = 0; // make sure the string is null terminated
        printf("%s",message);
    }
    return NULL;

}

int main (int argc, char** argv) {
    pthread_t tid_receiver;
    char message[MAX_SIG];
    int port;
    
    struct sockaddr_in addr;
    port = atoi(argv[2]);
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // on server

    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    if(connect(tcp_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        printf("bind failed\n");
        return EXIT_FAILURE;
    }
    pthread_create(&tid_receiver, NULL, receiver, NULL);

    send(tcp_socket , argv[1] , strlen(argv[1]) , 0 );

    while (strncmp(message,"/quit",5) != 0 && strncmp(message,"/shutdown",9) != 0) {
        fgets(message, MAX_SIG, stdin); 
        send(tcp_socket , message , strlen(message) , 0 );
    }
    // wait for the receiver thread to finish. socket close on the server will cause the thread to exit
    pthread_join(tid_receiver, NULL);
    return EXIT_SUCCESS;
    
}