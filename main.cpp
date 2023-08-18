#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <list>
#include <mutex>
#include <thread>

using namespace std;


// Shared data structures
list<int> clientSockets; // storing client IDs

// Synchronization of primitives
mutex clientListMutex;

int initialize_server_socket(struct sockaddr_in server_addr, socklen_t server_bind){
    int backlog = 5;

    int sockFD = socket(AF_INET, SOCK_STREAM, 0);

    if(sockFD == -1){
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }
    if(bind(sockFD, (struct sockaddr *)&server_addr,sizeof(server_addr)) == -1) {

        perror("binding error");
        exit(EXIT_FAILURE);
    }
    if(listen(sockFD, backlog) == -1){

        perror("listen error");
        exit(EXIT_FAILURE);
    }
    return sockFD;
}

// Add a new client to the socket list
void addClient(int clientSocket){

    // Automatically locks our mutex
    lock_guard<mutex> lock(clientListMutex);
    
    // Mutex will be automatically locked when lock goes out of scope
    clientSockets.push_back(clientSocket);
}

// Add a new client to the socket list
void removeClient(int clientSocket){

    // Automatically locks our mutex
    lock_guard<mutex> lock(clientListMutex);
    
    // Mutex will be automatically locked when lock goes out of scope
    clientSockets.remove(clientSocket);
}

void broadcastMessage(const char* message){

    lock_guard<mutex> lock(clientListMutex);
    for (int socket : clientSockets){
        send(socket, message, strlen(message), 0);
    }
}

void clientHandler(int clientSocket){
    addClient(clientSocket);

    while(true) {

        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0){
            // client disconnected or error
            break;
        }
        broadcastMessage(buffer);
    }
    removeClient(clientSocket);
    close(clientSocket);
}


int main() {
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int backlog = 5;

    int listeningSocket = initialize_server_socket(server_addr, client_addr_len);


    while(true) {

        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);

        int clientSocket = accept(listeningSocket, (struct sockaddr*)&client_addr, &client_addr_len);

        if (clientSocket == -1){
            perror("error accepting client");
            continue;
        }
        thread(clientHandler, clientSocket).detach();
    }
    close(listeningSocket);
    return 0;
}
    
    /*
    server_Listener(server_addr, socklen_t client_addr_len);

    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_port = htons(8080); // selecting port
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to local address

    int sockFD = socket(AF_INET, SOCK_STREAM, 0);

    if(sockFD == -1){
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }

    int result = bind(sockFD, (struct sockaddr *)&server_addr,sizeof(server_addr));

    int resultListen = listen(result, backlog);
    int resultAccept = accept(result, (struct sockaddr *)&client_addr, &client_addr_len);

    if (resultAccept != -1){
        const char* message = "Hello Server!";
        send(sockFD, message, strlen(message), 0);
        char buffer[1024];
        ssize_t bytes_received = recv(sockFD, buffer, sizeof(buffer), 0);
        close(sockFD);
    }

    list<int> clients; // storing client IDs
    mutex clientMutex;
    */
}
    