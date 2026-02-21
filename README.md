# Process Monitoring System (C)
A simple client-server system using TCP sockets and fork() in C.

## Features
- Handles multiple clients using fork()
- Commands supported:
  - `date` → shows system date via execl()
  - `list` → lists files using system("ls")
  - `loop` → demonstrates CPU scheduling
  - invalid → returns "Invalid Command"
- Prevents zombie processes with `signal(SIGCHLD, SIG_IGN)`

## How to run
```bash
gcc monitor_server.c -o server
gcc monitor_client.c -o client
./server &
./client
