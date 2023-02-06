#include <stdlib.h>
#include "../include/ConnectionHandler.h"
#include "../include/ClientReader.h"
#include "../include/StompProtocol.h"
#include "../include/User.h"
#include "../include/GameTracker.h"
#include <thread>


#include <unistd.h>
#include <iostream>

int main(int argc, char *argv[]) {
	ConnectionHandler connectionHandler("",0);
	StompProtocol protocol;
	User user;
	GameTracker gameTracker;
	ClientReader socket(connectionHandler, protocol, user, gameTracker);
	std::thread socketThread(&ClientReader::Run, &socket);
	
	while(1)
    {
        std::cout << "enter a command to take action:" << std::endl;
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize);
		std::string line(buf);

        std::vector<std::string> hostAndPort = protocol.isLoginCommand(line);
		

        if(hostAndPort.size() > 0 && !connectionHandler.getIsActive())
        {
            std::string host = hostAndPort.at(0);
            short port = stoi(hostAndPort.at(1));
            connectionHandler.setPort(port);
			connectionHandler.setHost(host);
            if (!connectionHandler.connect()) {
                std::cerr << "Could not connect to server" << std::endl;
				
            } else {
				connectionHandler.setIsActive(true);
			}               
			hostAndPort.clear();
        }
		if(hostAndPort.size() > 0) {
			std::cout << "The client is already logged in ,log out before trying again" << std::endl;
		} else if(connectionHandler.getIsActive()) {
			std::queue<std::string> frames = protocol.createFrame(line, user, gameTracker);
			//execute sendLine only!! if the command is correct and frame was built
			//for example if there was an error on client side, createframe will return "" and sendLIne wont be executed
			while(!frames.empty()) {
				std::string frame = frames.front();
				frames.pop();
				if (!connectionHandler.sendLine(frame)) {
					std::cout << "Disconnected. Exiting...\n" << std::endl;
				}
			}
		}
    }
	socketThread.join();

	return 0;
}