// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define PORT 8080

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *name = "Client No.1";
    char in[1024];
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    if (argc == 2)
        send(sock, argv[1], strlen(argv[1]), 0);
    else
        send(sock, name, strlen(name), 0);
    printf("[Accept]: Successful!\n");
    while(1) {
        memset(in, 0, 1024*sizeof(char));
        fgets(in, 1024 * sizeof(char), stdin);
        send(sock, in, strlen(in), 0);
        printf("-Sent√-");
        valread = read(sock, buffer, 1024);
        printf("[Read]: %s\n", buffer);
    }
    return 0;
}
