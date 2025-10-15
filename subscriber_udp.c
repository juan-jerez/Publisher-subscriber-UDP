// subscriber_udp.c
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
    struct sockaddr_in sub_addr, broker_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(sub_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        exit(EXIT_FAILURE);
    }

    sub_addr.sin_family = AF_INET;
    sub_addr.sin_addr.s_addr = INADDR_ANY;
    sub_addr.sin_port = htons(0);  // puerto aleatorio
    bind(sockfd, (struct sockaddr *)&sub_addr, sizeof(sub_addr));

    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(BROKER_PORT);
    inet_pton(AF_INET, BROKER_IP, &broker_addr.sin_addr);

    // Enviar solicitud de suscripción
    strcpy(buffer, "SUBSCRIBE");
    sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&broker_addr, sizeof(broker_addr));
    printf("👥 Suscriptor registrado en el broker\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&broker_addr, &addr_len);
        printf("📢 Actualización recibida: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
