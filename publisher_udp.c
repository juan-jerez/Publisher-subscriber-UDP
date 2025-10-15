// publisher_udp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROKER_IP "191.168.194.201"
#define BROKER_PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in broker_addr;
    char buffer[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(BROKER_PORT);
    inet_pton(AF_INET, BROKER_IP, &broker_addr.sin_addr);

    printf("📰 Publisher UDP listo para enviar mensajes...\n");

    while (1) {
        printf("Escribe un mensaje (o 'exit' para salir): ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "exit") == 0) break;

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&broker_addr, sizeof(broker_addr));
    }

    close(sockfd);
    return 0;
}
