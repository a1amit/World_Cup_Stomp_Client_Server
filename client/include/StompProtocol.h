#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/User.h"
#include "../include/GameTracker.h"
#include <boost/unordered_map.hpp>

#include <string>
#include <vector>
#include <queue>

using namespace std;


// TODO: implement the STOMP protocol
class StompProtocol
{
private:
    bool shouldTerminate;
    vector<string> split(string s, char del);
    string processLogin(vector<string> vec, User &user);
    string processJoin(vector<string> vec, User &user, GameTracker &gameTracker);
    string processExit(vector<string> vec, User &user, GameTracker &gameTracker);
    queue<string> processReport(vector<string> vec, User &user);
    void processSummary(vector<string> vec, GameTracker &gameTracker);
    string processLogout(vector<string> vec, User &user);
    string printfMap(const map<string, string> &map);
    void proccessMessageFrame(string frame, GameTracker &gameTracker);
    boost::unordered_map<string,string> extractAttributes(string start, string end, vector<string> frameVector);
public:
    StompProtocol();
    queue<string> createFrame(string command, User &user, GameTracker &gameTracker);
    string parseFrame(string frame, User &user, GameTracker &gameTracker);
    bool getShouldTerminate();
    void setShouldTerminate(bool value);
    vector<string> isLoginCommand(string command);
    bool isErrorFrame(string frame);
    int getReciptId(string frame);
};
