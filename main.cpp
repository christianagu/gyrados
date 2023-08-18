#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include <list>
#include <mutex>

using namespace std;

int main() {
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int backlog = 5;

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
}
    