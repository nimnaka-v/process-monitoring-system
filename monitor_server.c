#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define PORT 9001

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr;
    socklen_t addr_size;
    char buffer[1024];

    // Prevent zombie processes
    signal(SIGCHLD, SIG_IGN);

    // Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket failed");
        exit(1);
    }

    // Server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind failed");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf("Server started. PID: %d\n", getpid());

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        // Fork for concurrent client
        pid_t pid = fork();
        if (pid == 0) {  // child process
            close(server_fd); // child doesn’t need listening socket
            printf("New client connected (Child PID: %d | Parent PID: %d)\n", getpid(), getppid());

            while (1) {
                memset(buffer, 0, sizeof(buffer));
                int bytes = recv(client_fd, buffer, sizeof(buffer)-1, 0);
                if (bytes <= 0) break; // client disconnected

                buffer[bytes] = '\0'; // null-terminate
                printf("Client (%d) sent: %s\n", getpid(), buffer);

                // Handle commands
                if (strncmp(buffer, "date", 4) == 0) {
                    dup2(client_fd, STDOUT_FILENO);
                    dup2(client_fd, STDERR_FILENO);
                    execl("/bin/date", "date", NULL);
                    perror("execl failed");
                    break;
                } else if (strncmp(buffer, "list", 4) == 0) {
                    dup2(client_fd, STDOUT_FILENO);
                    dup2(client_fd, STDERR_FILENO);
                    system("ls");
                } else if (strncmp(buffer, "loop", 4) == 0) {
                    for (long i = 0; i < 1000000000; i++); // simulate CPU usage
                    send(client_fd, "Loop completed\n", 15, 0);
                } else {
                    send(client_fd, "Invalid Command\n", 16, 0);
                }
            }

            printf("Client disconnected. Child PID %d exiting.\n", getpid());
            close(client_fd);
            exit(0);
        }
        else if (pid > 0) { // parent process
            printf("Parent (%d) accepted a connection, spawned Child PID: %d\n", getpid(), pid);
            close(client_fd); // parent doesn’t handle client
        }
        else {
            perror("fork failed");
            close(client_fd);
        }
    }

    close(server_fd);
    return 0;
}

