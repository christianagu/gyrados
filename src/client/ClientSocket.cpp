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

void sendMessages(int clientSocket){
    string message;
    getline(cin, message);
     if (send(clientSocket, message.c_str(), message.length(), 0) == -1) {
        perror("Failed to send message");
    }
}


int initialize_client_socket(struct sockaddr_in client_addr, socklen_t client_connect){
    int backlog = 5;

    int sockFD = socket(AF_INET, SOCK_STREAM, 0);

    if(sockFD == -1){
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }
    if(connect(sockFD, (struct sockaddr *)&client_addr,sizeof(client_addr)) == -1) {

        perror("connecting error");
        exit(EXIT_FAILURE);
    }
    return sockFD;
}

void receiveMessages(int clientSocket) {
    while (true) {
        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0){
            // Handle disconnection or error
            cout << "Disconnected from server or error occured";
            break;
        }
        buffer[bytesRead] = '\0'; // Null-terminate the string
        cout << "received: " << buffer << endl;
    }
}


int main() {
    struct sockaddr_in client_addr;

    memset(&client_addr, 0, sizeof(client_addr));

    // Set the client family, IP address, and port

    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(8080); // select the same port as server
    inet_pton(AF_INET, "127.0.0.1", &client_addr.sin_addr);

    int clientSocket = initialize_client_socket(client_addr, sizeof(client_addr));

    thread receiveThread(receiveMessages, clientSocket);
    receiveThread.detach();

    while (true) {

        sendMessages(clientSocket);
    }
    close(clientSocket);
    return 0;

}