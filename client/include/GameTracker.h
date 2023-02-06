#pragma once

#include <string>
#include <boost/unordered_map.hpp>
#include "../include/Game.h"

using namespace std;

class GameTracker 
{
    public:
        GameTracker();
        string createGameSummary(string gameName, string username);
        void addGametoTracker(string gameName);
        void removeGameFromTracker(string gameName);
        void addUpdate(string username, string gameName, boost::unordered_map<string,string> generalStats, boost::unordered_map<string,string> teamAStats, boost::unordered_map<string,string> teamBStats, string event);
        void resetGameTracker();
    private:
        boost::unordered_map<string ,boost::unordered_map<string, Game>> tracker;

};