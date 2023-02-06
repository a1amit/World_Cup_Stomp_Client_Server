#include "../include/User.h"

using namespace std;

User::User() : receiptIdCounter(0), subIdCounter(0), username(""), receipts(), subscribersIdsToGame(), gameSubscribersById()
{
}

int User::increaseUserReciptId()
{
    return receiptIdCounter++;
}

int User::increaseUserSubscriberId()
{
    return subIdCounter++;
}

string User::getUsername()
{
    return username;
}

void User::setUserName(string newUsername)
{
    username = newUsername;
}

void User::addReciept(int receiptId, string frameType)
{
    receipts[receiptId] = frameType;
}

string User::getReciept(int receiptId)
{
    return receipts[receiptId];
}

void User::removeReciept(int receiptId)
{
    receipts.erase(receiptId);
}

void User::addSubId2Game(int subId, string gameName)
{
    subscribersIdsToGame[subId] = gameName;
}

string User::getSubscriberIdToGame(int subId)
{
    return subscribersIdsToGame[subId];
}

void User::removeSubscriberIdToGame(int subId)
{
    subscribersIdsToGame.erase(subId);
}

void User::addSubscriberIdToGame(string gameName, int subId)
{
    gameSubscribersById[gameName] = subId;
}

int User::getSubscriberIdFromGame(string gameName)
{
    if(gameSubscribersById.count(gameName) > 0) {
        return gameSubscribersById[gameName];    
    }
    return -1;
    
}

void User::removeSubscriberFromGameByName(string gameName)
{
    gameSubscribersById.erase(gameName);
}

void User::resetUserParameters()
{
    receiptIdCounter = 0;
    username = "";
    subIdCounter = 0;
    //add reset to the dicts
    gameSubscribersById.clear();
    subscribersIdsToGame.clear();
    receipts.clear();
}