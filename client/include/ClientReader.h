#pragma once

#include "../include/GameTracker.h"
#include "../include/User.h"
#include "../include/ConnectionHandler.h"
#include "../include/StompProtocol.h"




class ClientReader
{
public:
    ClientReader(ConnectionHandler &connectionHandler, StompProtocol &protocol,
     User &user, GameTracker &gameTracker);
    void Run();

private:
    ConnectionHandler &connectionHandler;
    bool shouldTerminate;
    StompProtocol &protocol;
    User &user;
    GameTracker &gameTracker;
};
