// publisher_udp.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BROKER_IP "192.168.100.10"  // Cambia por la IP del broker
#define BROKER_PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in broker_addr;
    char buffer[BUFFER_SIZE];
    char topic[64];
    char message[BUFFER_SIZE];

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(BROKER_PORT);
    inet_pton(AF_INET, BROKER_IP, &broker_addr.sin_addr);

    printf("📰 Publisher UDP conectado al broker %s:%d\n", BROKER_IP, BROKER_PORT);

    printf("Ingrese el topic (ej: PARTIDO1): ");
    fgets(topic, sizeof(topic), stdin);
    topic[strcspn(topic, "\n")] = '\0';

    while (1) {
        printf("Mensaje (o 'exit'): ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0';
        if (strcmp(message, "exit") == 0) break;

        char packet[BUFFER_SIZE];
        snprintf(packet, sizeof(packet), "%s|%s", topic, message);

        sendto(sockfd, packet, strlen(packet), 0,
               (struct sockaddr *)&broker_addr, sizeof(broker_addr));
    }

    close(sockfd);
    return 0;
}
