#pragma once

#include <string>
#include <boost/unordered_map.hpp>

using namespace std;

class User
{
    private:
        int receiptIdCounter;
        int subIdCounter;
        string username;
        boost::unordered_map<int,string> receipts;
        boost::unordered_map<int,string> subscribersIdsToGame;
        boost::unordered_map<string, int> gameSubscribersById;


    public:
        User();
        int increaseUserReciptId();
        int increaseUserSubscriberId();
        void setUserName(string newUsername);
        void removeSubscriberIdToGame(int subId);
        string getUsername();
        void resetUserParameters();
        void addSubscriberIdToGame(string gameName, int subId);
        void addReciept(int recieptId, string frameType);
        string getReciept(int receiptId);
        void removeReciept(int receiptId);
        void addSubId2Game(int subId, string gameName);
        string getSubscriberIdToGame(int subId);
        int getSubscriberIdFromGame(string gameName);
        void removeSubscriberFromGameByName(string gameName);

        
};