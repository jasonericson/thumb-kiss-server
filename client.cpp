// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8080
#define MAXLINE 1024

int sockfd;
struct sockaddr_in servaddr;
int other_client_value;

void* recv_from_other_client(void* ptr)
{
    char buffer[MAXLINE];
    socklen_t len;
    int n;

    len = sizeof(servaddr);
    while (1)
    {
        n = recvfrom(sockfd, (char*)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr *) &servaddr,
                     &len);
        buffer[n] = '\0';
        other_client_value = atoi(buffer);
    }
}

// Driver code
int main(int argc, char* argv[])
{
    char id;
    char buffer[MAXLINE];
    char msg[32];
    pthread_t recv_thread;

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

    pthread_create(&recv_thread, NULL, recv_from_other_client, NULL);

    socklen_t len;
    int n, thing;
    thing = 0;

    while (1)
    {
        len = sizeof(servaddr);
        sprintf(msg, "%c%d", id, thing);
        sendto(sockfd, (const char *)msg, strlen(msg),
               MSG_CONFIRM, (const struct sockaddr *) &servaddr,
               len);
        printf("Message sent with %d.\n", thing);
        printf("Value from other client: %d\n", other_client_value);
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
