#include "../include/ClientReader.h"

ClientReader::ClientReader(ConnectionHandler &connectionHandler, StompProtocol &protocol, User &user, GameTracker &gameTracker) : connectionHandler(connectionHandler), shouldTerminate(false), protocol(protocol), user(user), gameTracker(gameTracker)
{

}

void ClientReader::Run()
{
    while(!shouldTerminate)
    {
        std::string answer;
    
        if(connectionHandler.getIsActive()) 
        {
            if (!connectionHandler.getLine(answer)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            //parse answer and return the reply according to it
            std::string replyToClient = protocol.parseFrame(answer, user, gameTracker);

            if(protocol.isErrorFrame(answer))
            {
                user.resetUserParameters();
                connectionHandler.close();
                connectionHandler.setIsActive(false);
                gameTracker.resetGameTracker();
            } else
            {
                int rId = protocol.getReciptId(answer);
                if(rId != -1)
                {
                    //check the type of the receipt in user
                    //if "logout" - reset user, disconnect (handlerInit= false) from handler
                    std::string frameType = user.getReciept(rId);
                    if(frameType.find("logout") != std::string::npos)
                    {
                        user.resetUserParameters();
                        connectionHandler.close();
                        connectionHandler.setIsActive(false);
                        gameTracker.resetGameTracker();
                    } else
                    {
                        user.removeReciept(rId);
                    }
                }                
            }
            std::cout << "Reply: " << replyToClient << std::endl;

        }
    }
}