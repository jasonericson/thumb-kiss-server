// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024

// Driver code
int main(int argc, char* argv[])
{
    char id;
    int sockfd;
    char buffer[MAXLINE];
    char msg[32];
    struct sockaddr_in servaddr;

    if (argc != 2)
    {
        perror("Wrong number of arguments.");
        exit(EXIT_FAILURE);
    }

    id = argv[1][0];

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    int n, len, thing;
    thing = 0;

    while (1)
    {
        sprintf(msg, "%c%d", id, thing);
        sendto(sockfd, (const char *)msg, strlen(msg),
               MSG_CONFIRM, (const struct sockaddr *) &servaddr,
               sizeof(servaddr));
        printf("Message sent with %d.\n", thing);
        ++thing;

        sleep(1);
    }

    // n = recvfrom(sockfd, (char *)buffer, MAXLINE,
    //              MSG_WAITALL, (struct sockaddr *) &servaddr,
    //              &len);
    // buffer[n] = '\0';
    // printf("Server: %s\n", buffer);

    close(sockfd);
    return 0;
}
