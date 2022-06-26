#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

#define PORT    1234
#define MAXLINE 1024

#define VERBOSE 0

int main()
{
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;
    struct sockaddr_in *wanda_addr, *jason_addr;
    char msg[32];

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    wanda_addr = NULL;
    jason_addr = NULL;

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr*)&servaddr,
             sizeof(servaddr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    socklen_t len;
    int n;

    while (1)
    {
        len = sizeof(cliaddr); // len is value/result
        n = recvfrom(sockfd, (char*)buffer, MAXLINE,
                     MSG_WAITALL, (struct sockaddr*) &cliaddr,
                     &len);

        buffer[n] = '\0';

        rapidjson::Document in_doc;
        in_doc.Parse(buffer);

        if (in_doc.GetObject()["id"].GetString()[0] == 'w')
        {
            // From Wanda
            if (wanda_addr == NULL)
            {
                wanda_addr = (sockaddr_in*)malloc(len);
            }

            memcpy(wanda_addr, &cliaddr, len);

#if VERBOSE
            printf("From Wanda (port %d): %s\n", wanda_addr->sin_port, buffer);
#endif

            if (jason_addr != NULL)
            {
                sendto(sockfd, buffer, strlen(buffer),
                    0, (const struct sockaddr *) jason_addr,
                    len);
#if VERBOSE
                printf("To Jason (port %d): %s\n", jason_addr->sin_port, buffer);
#endif
            }
        }
        else
        {
            // From Jason
            if (jason_addr == NULL)
            {
                jason_addr = (sockaddr_in*)malloc(len);
            }

            memcpy(jason_addr, &cliaddr, len);
#if VERBOSE
            printf("Jason (port %d): %s\n", jason_addr->sin_port, buffer);
#endif

            if (wanda_addr != NULL)
            {
                sendto(sockfd, buffer, strlen(buffer),
                    0, (const struct sockaddr *) wanda_addr,
                    len);
#if VERBOSE
                printf("To Wanda (port %d): %s\n", wanda_addr->sin_port, buffer);
#endif
            }
        }
    }

    return 0;
}
