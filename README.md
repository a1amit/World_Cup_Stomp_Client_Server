# World Cup Game Updates Stomp Client-Server

This project is a Stomp (Streaming Text Oriented Messaging Protocol) client-server application that provides real-time updates on World Cup game events. The game events are provided in JSON format. The client is implemented in C++, while the server is implemented in Java. The server supports two types of server architectures: TPC (Transmission Control Protocol) and Reactor.

## Features

- Real-time updates: The server broadcasts game event updates to all connected clients in real-time.
- JSON support: The server parses game events from JSON files and sends them to the clients.
- Stomp protocol: The client-server communication follows the Stomp protocol, allowing clients to subscribe to specific game events and receive updates.
- TCP server: The project includes a TCP server implementation that handles client connections and message routing.
- Reactor server: The project also includes a Reactor server implementation that uses a single-threaded event loop for handling client requests.

## Supported Actions

- Server Frames:
**CONNECTED**
**MESSAGE**
**RECEIPT**
**Error**
- Client Frames:
**CONNECT**
**SEND**
**SUBSCRIBE**
**UNSUBSCRIBE**
**DISCONNECT**

- for more inforamtion see the instruction.pdf file

## Getting Started

To run the World Cup Game Updates Stomp Client-Server, follow these steps:

1. Clone the repository: `git clone https://github.com/your-repo-url.git`
2. Compile the server: `javac Server.java`
3. Run the server: `java Server <server-type>`
   - Replace `<server-type>` with either `TCP` or `Reactor`, depending on the server architecture you want to use.
4. Compile the client: `g++ Client.cpp -o client`
5. Run the client: `./client <server-address> <server-port>`
   - Replace `<server-address>` and `<server-port>` with the appropriate server address and port.

- for more inforamtion see the instruction.pdf file

## Usage

Once the server and client are running, you can use the following commands in the client to interact with the server:

- `login <username> <password>`: Logs in to the server with the specified username and password.
- `subscribe <game-name>`: Subscribes to receive updates on the specified game.
- `report <events-file>`: Reports game events from the specified JSON file to the server.
- `summary <game-name> <user> <file>`: Prints a summary of game updates for the specified game and user into the provided file.
- `logout`: Logs out from the server and terminates the client connection.

- for more inforamtion see the instruction.pdf file


## Acknowledgments

- The Stomp protocol and supported actions are based on the instructions provided in the project documentation.
