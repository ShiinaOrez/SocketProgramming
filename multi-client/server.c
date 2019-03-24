//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
	
#define TRUE 1 
#define FALSE 0 
#define PORT 8080

struct Message{
	int from_sfd;
	char content[1024];
}buf_m;

void initClientSocketSet(int client_socket[], int size) {
    int i;
    for (i=0; i<size; i++)
        client_socket[i] = 0;
}

void setAddress(struct sockaddr_in *address, int family, int addr, int port) {
    (*address).sin_family = family;
    (*address).sin_addr.s_addr = addr;
    (*address).sin_port = htons(port);
}

int checkMaxSocketDescriptor(int client_socket[], int size, fd_set *readfds, int master) {
    int max_sd = master;
    int sd = 0, i;
    //add child sockets to set
    for (i=0; i<size; i++) {
        //socket descriptor
        // now the child socket is 0, the init statu
        sd = client_socket[i];

        //if valid socket descriptor then add to read list
        if(sd > 0)
            FD_SET(sd ,readfds);

        //highest file descriptor number, need it for the select function
        if(sd > max_sd)
            max_sd = sd;
    }
    return max_sd;
}
	
int main(int argc , char *argv[]) {
	int opt = TRUE; 
	int master_socket, addrlen, new_socket, client_socket[30], max_clients=30, activity, i, valread, sd;
	int max_sd; 
	struct sockaddr_in address; 
		
	char buffer[1025]; //data buffer of 1K
	char name[30][100]={0};
		
	//set of socket descriptors 
	fd_set readfds; 
		
	//a message 
	char *message = "[Server]: Connected!\n";
	char *success = "Successful!\n";

	//initialise all client_socket[] to 0 so not checked 
	initClientSocketSet(client_socket, max_clients);

	//create a master socket
	if((master_socket=socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("[Socket]: Master Socket Create Failed!");
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this
	// the different of before version is SO_REUSEPORT
	if(setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
		perror("[Socket]: Set Socket Option Failed!");
		exit(EXIT_FAILURE); 
	} 
	
	//type of socket created
	setAddress(&address, AF_INET, INADDR_ANY, PORT);
		
	//bind the socket to localhost port 8080
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
		perror("[Socket]: Socket Bind to Localhost Failed");
		exit(EXIT_FAILURE); 
	} 
	printf("[Listen]: Listening on Port: %d\n", PORT);

	//try to specify maximum of 4 pending connections for the master socket
	if (listen(master_socket, 4) < 0) {
		perror("[Listen]: Listen Failed!");
		exit(EXIT_FAILURE); 
	} 

	//accept the incoming connection 
	addrlen = sizeof(address); 
	printf("Waiting for connections ...\n");
		
	while(TRUE) {
		//clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(master_socket, &readfds);

		max_sd = checkMaxSocketDescriptor(client_socket, max_clients, &readfds, master_socket);
		//wait for an activity on one of the sockets, timeout is NULL ,
		//so wait indefinitely

		// int select(int maxfd+1, fd_set *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout);
		// maxfd+1(file descriptor count), because of file descriptor start with 0, so the number of fds is the max_fd + 1.
		// readset, ready for check read ability
		// writeset, ready for check write ability
		// exceptset, ready for check the exception
		// timeout
		activity = select(max_sd+1, &readfds, NULL, NULL, NULL);
	    //if activity == 0, timeout
		if ((activity < 0) && (errno!=EINTR))
			printf("[Select]: Select Error!");
			
		//If something happened on the master socket , 
		//then its an incoming connection
		// judge the connection request
		if (FD_ISSET(master_socket, &readfds)) {
			if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
				perror("[Accept]: Accept Failed!");
				exit(EXIT_FAILURE); 
			}
			valread = read(new_socket, name[new_socket], 100);
			//inform user of socket number - used in send and receive commands 
			printf("[Client]: Socket: %d|IP: %s|Port: %d|Aka: %s\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port), name[new_socket]);
		
			//send new connection greeting message 
			if( send(new_socket, success, strlen(message), 0) != strlen(message))
				perror("[Client]: Send Message Failed!");
				
			printf("[Client]: Connected!\n");
				
			//add new socket to array of sockets 
			for (i=0; i<max_clients; i++) {
				//if position is empty 
				if( client_socket[i] == 0 ) {
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n", i);
					break; 
				} 
			} 
		}

		//Check News
			
		//else its some IO operation on some other socket 
		for (i=0; i<max_clients; i++) {
			sd = client_socket[i]; 
				
			if (FD_ISSET(sd , &readfds)) {
				//Check if it was for closing , and also read the 
				//incoming message
				memset(buffer, 0, 1024*sizeof(char));
				if ((valread = read(sd, buffer, 1024)) == 0) {
					//Somebody disconnected , get his details and print 
					getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
					printf("[Client]: Disconnected|IP: %s|Port %d|Aka: %s", inet_ntoa(address.sin_addr) , ntohs(address.sin_port), name[sd]);
						
					//Close the socket and mark as 0 in list for reuse 
					close(sd);
					client_socket[i] = 0; 
				} 
					
				//Echo back the message that came in 
				else {
					//set the string terminating NULL byte on the end 
					//of the data read

					buffer[valread] = '\0';
					printf("%d|%s: %s", sd, name[sd], buffer);

					if (!strcmp(buffer, "show me the table\n")) {
					    int j;
					    for (j=0; j<max_clients; j++) {
					        if (client_socket[j] != 0) {
					            char *aka = name[client_socket[j]];
					            char *msg = "|";
					            msg = strcat(aka, msg);
					            send(sd, msg, strlen(msg), 0);
					        }
					    }
					}

					send(sd, success, strlen(success), 0);
					buf_m.from_sfd = sd;
					strcpy(buffer, buf_m.content);
				}
			} 
		} 
	}
	return 0; 
}