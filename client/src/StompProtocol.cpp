#include "../include/StompProtocol.h"
#include "../include/event.h"
#include <queue>
#include <fstream>

using namespace std;

StompProtocol::StompProtocol() : shouldTerminate(false) {}

std::queue<std::string> StompProtocol::createFrame(std::string command, User &user, GameTracker &gameTracker) 
{
    //we assume the command input is legal
    vector<string> frameVector = split(command, ' ');
    string userCommand = frameVector.at(0);
    queue<string> outputQueue;
    if(userCommand == "login") 
    {
        outputQueue.push(processLogin(frameVector, user));
    } else if (userCommand == "join")
    {
        outputQueue.push(processJoin(frameVector, user, gameTracker));
    } else if (userCommand == "exit")
    {
        outputQueue.push(processExit(frameVector, user, gameTracker));
    } else if (userCommand == "report")
    {
        outputQueue = processReport(frameVector, user);
    } else if (userCommand == "summary")
    {
        processSummary(frameVector, gameTracker);
    } else if (userCommand == "logout")
    {
        outputQueue.push(processLogout(frameVector, user));
    } 

    return outputQueue;
}

std::string StompProtocol::parseFrame(std::string frame, User &user, GameTracker &gameTracker)
{
    vector<string> frameVector = split(frame, '\n');
    string userCommand = frameVector.at(0);
    string output = "";
    if(userCommand == "CONNECTED") {
        output = "login successful";
    } else if (userCommand == "RECEIPT") {
        int rId = getReciptId(frame);
        output = user.getReciept(rId);
    } else if (userCommand == "ERROR") {
        output = frameVector.at(frameVector.size() - 2); 
    } else if (userCommand == "MESSAGE") {
        proccessMessageFrame(frame, gameTracker);
        output = frame;
    }
    return output;
}

void StompProtocol::proccessMessageFrame(std::string frame, GameTracker &gameTracker)
{
    std::vector<std::string> splitVec = split(frame, '\n');
    
    std::string username = "";
    std::string teamAname = "";
    std::string teamBname = "";
    string eventName = "";
    string time = "";

    for(string line : splitVec){
        std:: string key, value;
        std::stringstream ss(line);
        getline(ss,key,':');
        getline(ss,value);
        if(key == "user"){
            username = value;
        }
        if(key == "team a"){
            teamAname = value;
        }
        if(key == "team b"){
            teamBname = value;
        }
        if(key == "event name"){
            eventName = value;
        }
        if(key == "time"){
            time = value;
        }
    }

    
    
    
    std::string gameName = teamAname + "_" + teamBname;

    boost::unordered_map<std::string,std::string> generalStats = extractAttributes("general game updates:", "team a updates:", splitVec);
    boost::unordered_map<std::string,std::string> teamAstats = extractAttributes("team a updates:", "team b updates:", splitVec);
    boost::unordered_map<std::string,std::string> teamBstats = extractAttributes("team b updates:", "description:", splitVec);

    string desc = splitVec.at(14 + generalStats.size() + teamAstats.size() + teamBstats.size());

    string event = time + " - " + eventName + "\n" + desc;

    gameTracker.addUpdate(username, gameName, generalStats, teamAstats, teamBstats, event);
}

boost::unordered_map<std::string,std::string> StompProtocol::extractAttributes(std::string start, std::string end, std::vector<std::string> frameVector)
{
    int indexStart = 0;
    for (unsigned int i = 0; i < frameVector.size(); i++){
        if (frameVector.at(i) == start){
            indexStart = i + 1;
            break;
        }
    }

    int indexEnd = 0;
    for (unsigned int i = indexStart; i < frameVector.size(); i++){
        if (frameVector.at(i) == end){
            indexEnd = i;
            break;
        }
    }

    boost::unordered_map<std::string,std::string> output;

    for (int i = indexStart; i < indexEnd; i++){
        vector<string> attAndVal = split(frameVector.at(i), ':');
        output[attAndVal.at(0).substr(1)] = attAndVal.at(1).substr(1);
    }
    
    return output;
}



bool StompProtocol::getShouldTerminate()
{
    return shouldTerminate;
}

void StompProtocol::setShouldTerminate(bool value)
{
    shouldTerminate = value;
}   


std::vector<std::string> StompProtocol::split(std::string s, char del)
{
    std::vector<std::string> vector;

    std::stringstream ss(s);
    std::string word;
    while(!ss.eof()) 
    {
        std::getline(ss, word, del);
        vector.push_back(word);
    }

    return vector;
}

string StompProtocol::processLogin(vector<string> vec, User &user)
{
    //build CONNECT frame
    // return "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:" + vec.at(2) + "\npasscode:" + vec.at(3) + "\n\n" + '\0';
    user.setUserName(vec.at(2));

    return "CONNECT\naccept-version:1.2\nhost:stomp.cs.bgu.ac.il\nlogin:" + 
            vec.at(2) + "\npasscode:" + vec.at(3) + "\n\n"; 
            //removed null char bc sendAsciiFrame adds it automatically
} 


string StompProtocol::processJoin(vector<string> vec, User &user, GameTracker &gameTracker)
{
    int rId = user.increaseUserReciptId();
    int sId = user.increaseUserSubscriberId();
    std::string gameName = vec.at(1);
    user.addReciept(rId, "joined " + gameName);
    user.addSubId2Game(sId, gameName);
    user.addSubscriberIdToGame(gameName, sId);
    gameTracker.addGametoTracker(gameName);
    return "SUBSCRIBE\ndestination:/" + gameName + "\nid:" + std::to_string(sId) + "\nreceipt:" + std::to_string(rId) + "\n\n";
} 

string StompProtocol::processExit(vector<string> vec, User &user, GameTracker &gameTracker)
{
    int rId = user.increaseUserReciptId();
    std::string gameName = vec.at(1);
    user.addReciept(rId, "Exited channel" + gameName);
    int sId = user.getSubscriberIdFromGame(gameName);
    if(sId >= 0) {
        user.removeSubscriberIdToGame(sId);
        user.removeSubscriberFromGameByName(gameName);
        gameTracker.removeGameFromTracker(gameName);
    }
    return "UNSUBSCRIBE\nid:" + std::to_string(sId) + "\nreceipt:" + std::to_string(rId) + "\n\n";
} 

queue<string> StompProtocol::processReport(vector<string> vec, User &user)
{
    names_and_events namesAndEvents = parseEventsFile("../data/" + vec.at(1));

    queue<string> output;
    for(Event e : namesAndEvents.events)
    {
        string frame = "SEND\ndestination:/" +
         e.get_team_a_name() + 
         "_" + e.get_team_b_name() 
         + "\n\nuser: " + 
         user.getUsername() + 
         "\nteam a: " + e.get_team_a_name() 
         + "\nteam b: " + e.get_team_b_name() 
         + "\nevent name: " + e.get_name() + 
         "\ntime: " + 
         std::to_string(e.get_time()) 
         + "\ngeneral game updates:" + 
         printfMap(e.get_game_updates()) + 
         "\nteam a updates:" + 
         printfMap(e.get_team_a_updates()) + 
         "\nteam b updates:" + printfMap(e.get_team_b_updates()) + 
         "\ndescription:\n" + e.get_discription() + "\n" ;
        output.push(frame);
    }

    return output;
} 

std::string StompProtocol::printfMap(const std::map<std::string, std::string> &map)
{
    string output = "\n";
    for(pair<string, string> key : map)
    {
        output += '\t' + key.first + ": " + key.second + "\n";
    }
    if(output.size() > 0) 
    {
        output.resize(output.size() - 1);
    }
    return output;
}


void StompProtocol::processSummary(vector<string> vec, GameTracker &gameTracker)
{
    std::string summary = gameTracker.createGameSummary(vec.at(1), vec.at(2));
    string path = "../data/" + vec.at(3);
    std::ifstream infile(path);
    if(infile.good()){
        std::ofstream file("../data/" + vec.at(3));
        file << summary;
        file.close();
    } else {
        std::ofstream outfile(path);
        outfile << summary;
        outfile.close();
    }
    
    
} 

string StompProtocol::processLogout(vector<string> vec, User &user)
{
    int rId = user.increaseUserReciptId();
    user.addReciept(rId, "logout successful");
    
    return "DISCONNECT\nreceipt: " + std::to_string(rId) + "\n\n";
} 

vector<string> StompProtocol::isLoginCommand(std::string command)
{
    vector<string> frameVector = split(command, ' ');
    vector<string> output;
    if(frameVector.at(0) == "login")
    {
        return split(frameVector.at(1), ':');
    }

    return output;
}

int StompProtocol::getReciptId(std::string frame)
{
    vector<string> frameVector = split(frame, '\n');
    if(frameVector.at(0) == "RECEIPT") {
        return stoi(frameVector.at(1).substr(11));
    }
    return -1;
}

bool StompProtocol::isErrorFrame(std::string frame)
{
    vector<string> frameVector = split(frame, '\n');
    return frameVector.at(0) == "ERROR";
}
