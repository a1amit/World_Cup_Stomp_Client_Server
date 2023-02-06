#include "../include/Game.h"

Game::Game() : generalGameUpdates(), teamAUpdates(), teamBUpdates(), events()
{}

Game::Game(boost::unordered_map<std::string,std::string> generalStats, boost::unordered_map<std::string,std::string> teamAStats, boost::unordered_map<std::string,std::string> teamBStats) : generalGameUpdates(generalStats), teamAUpdates(teamAStats), teamBUpdates(teamBStats), events()
{}


std::vector<std::string> Game::printGameStats() 
{
    std::vector<std::string> prints;
    prints.push_back(printfMap(generalGameUpdates));
    prints.push_back(printfMap(teamAUpdates));
    prints.push_back(printfMap(teamBUpdates));
    return prints;

}

std::string Game::printfMap(const boost::unordered_map<std::string,std::string> &map)
{
    std::string output = "";
    for(std::pair<std::string, std::string> key : map)
    {
        output += '\t' + key.first + ": " + key.second + "\n";
    }
    if(output.size() > 0) 
    {
        output.resize(output.size() - 1);
    }
    return output;
}

std::string Game::printEvents()
{
    std::string output = "";
    for(std::string event : events)
    {
        output += event + "\n";
    }
    return output;
}

void Game::addEvent(std::string event)
{
    events.push_back(event);
}

void Game::updateGameStats(boost::unordered_map<std::string,std::string> generalStats, boost::unordered_map<std::string,std::string> teamAStats, boost::unordered_map<std::string,std::string> teamBStats)
{
    for(std::pair<std::string, std::string> pair : generalStats)
    {
        generalGameUpdates[pair.first] = pair.second;
    }

    for(std::pair<std::string, std::string> pair : teamAStats)
    {
        teamAUpdates[pair.first] = pair.second;
    }

    for(std::pair<std::string, std::string> pair : teamBStats)
    {
        teamBUpdates[pair.first] = pair.second;
    }
}


