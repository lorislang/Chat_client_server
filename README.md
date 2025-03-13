```markdown
# Multi-Client Chat Application

This project is a multi-client chat application implemented in C using TCP sockets and POSIX threads. It consists of a server that handles multiple clients and a client program that connects to the server.

## Features

- Multiple clients can connect to the server simultaneously.
- Clients can send messages to all other connected clients.
- Private messaging with the `/w <username> <message>` command.
- Clients can disconnect using `/quit`.
- The server can shut down all connections using `/shutdown`.

## Files

- `task3_client.c`: Client implementation for Task 3.
- `task3_server.c`: Server implementation for Task 3.
- `Makefile`: Compilation instructions.

## Compilation

To compile the project, use:

```sh
make
```

To clean the compiled binaries:

```sh
make clean
```

## Usage

### Start the Server

```sh
./task3_server <port>
```

### Start a Client

```sh
./task3_client <username> <port>
```

### Commands

- `/quit` – Disconnects the client.
- `/shutdown` – Shuts down the server and disconnects all clients.
- `/w <username> <message>` – Sends a private message to a specific user.

## Implementation Details

### Server

- Listens for incoming connections and spawns a new thread for each client.
- Maintains a list of connected clients.
- Broadcasts messages to all clients except the sender.
- Supports private messaging.
- Handles client disconnections and removes them from the active client list.

### Client

- Connects to the server and sends the username upon connection.
- Runs a separate thread to listen for messages from the server.
- Reads user input and sends messages to the server.
- Supports private messaging and disconnection commands.

## Dependencies

- POSIX Threads (`-pthread`)
- Real-Time Library (`-lrt`)

## Notes

- The server runs indefinitely until manually stopped or a client sends `/shutdown`.
- The maximum number of clients is defined as `MAX_CLIENTS` in the server code.
- The message buffer size is defined as `MAX_SIG`.

