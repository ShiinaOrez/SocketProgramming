#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    int server_fd, new_socket, valread;
    /*
     * struct sockaddr_in {
     *     short sin_family;          AF_INET or PF_INET
     *     unsigned short sin_port;   portNumber, normal number can convert by htons()
     *     struct in_addr, sin_addr;  IP address in network byte order(Internet address)
     *     unsigned char sin_zero[8]; Nothing= =
     * }
    */
    struct sockaddr_in address;
    int opt = 1; // means true
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char name[1024] = {0};
    char *receive = "Message Received!";
    char *goodbye = "[Over]: Good Bye~";

    // Creating socket file descriptor
    // int socket_file_descriptor = socket(domain, type, protocol)
    // domain: AF_INET(type: integer) means IPv4 protocol, if you want to use IPv6, use the AF_INET6
    // type: SOCK_STREAM means TCP, ConnectionOriented
    //       SOCK_DGRAM  means UDP, ConnectionLess
    // protocol: 0 means IP
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("[Socket]: Create Socket File Descriptor Failed!");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    // int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
    // sockfd: socket file descriptor
    // level: SOL_SOCKET(basic socket), IPPROTO_IP, IPPROTO_IPV6, IPPROTO_TCP
    // optname: if you choose SOL_SOCKET, SO_REUSEADDR means allowed to reuse local address and port.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("[Socket]: Set Socket Option Failed!");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port
    // bind the socket to host&port
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); /bind the socket file descriptor to address in.
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("[Bind]: Bind Failed!");
        exit(EXIT_FAILURE);
    }
    // int listen(int sockfd, int backlog)
    // backlog(type: integer) means the length of queue of pending connections
    printf("[Listen]: We're listening...\n");
    if (listen(server_fd, 4) < 0) {
        perror("[Listen]: Something Wrong!");
        exit(EXIT_FAILURE);
    }
    // int new_socket = accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    // select the first connection request in queue and accept it.
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("[Accept]: Accept Error!");
        exit(EXIT_FAILURE);
    }
    valread = read(new_socket, name, 1024);
    printf("[Accept]: Successful!\n");
    printf("[Client Name Is: %s]\n", name);
    while(1){
        memset(buffer, 0, 1024 * sizeof(char));
        valread = read(new_socket, buffer, 1024);
        if (strcmp(buffer, "exit\n") == 0)
            break;
        printf("|%s: %s", name, buffer);
        send(new_socket, receive, strlen(receive), 0);
    }
    send(new_socket, goodbye, strlen(goodbye), 0);
    printf("[Over]: Good Bye~\n");
    return 0;
}
