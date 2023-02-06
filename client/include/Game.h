#pragma once

#include <string>
#include <boost/unordered_map.hpp>
#include <boost/container/vector.hpp>
#include <vector>
using namespace std;

class Game 
{
    public:
        Game();
        Game(boost::unordered_map<string,string> generalStats, 
        boost::unordered_map<string,string> teamAStats, 
        boost::unordered_map<string,string> teamBStats);
        vector<string> printGameStats();
        string printEvents();
        void addEvent(string event);
        void updateGameStats(boost::unordered_map<string,string> generalStats, boost::unordered_map<string,string> teamAStats, boost::unordered_map<string,string> teamBStats);
    private:
        boost::unordered_map<string,string> generalGameUpdates;
        boost::unordered_map<string,string> teamAUpdates;
        boost::unordered_map<string,string> teamBUpdates;
        boost::container::vector<string> events;
        string printfMap(const boost::unordered_map<string,string> &map);
};