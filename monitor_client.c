// monitor_client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 9001

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char message[1024];
    char buffer[2048];

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(1);
    }

    printf("Connected to server.\n");
    printf("Enter command (date / list / loop): ");
    scanf("%s", message);

    // ===== TASK 6 =====
    // Send request to server
    send(sock, message, strlen(message), 0);

    // ===== TASK 7 =====
    // Receive response from server
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0'; // null-terminate
        printf("\nServer response:\n%s\n", buffer);
    } else {
        printf("No response or connection closed.\n");
    }

    close(sock);
    return 0;
}
