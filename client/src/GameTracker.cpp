#include "../include/GameTracker.h"

GameTracker::GameTracker() : tracker() {}

std::string GameTracker::createGameSummary(std::string gameName, std::string username)
{
    std::vector<std::string> teamNames;

    std::stringstream ss(gameName);
    std::string word;
    while(!ss.eof()) 
    {
        std::getline(ss, word, '_');
        teamNames.push_back(word);
    }

    std::string teamAName = teamNames.at(0);
    std::string teamBName = teamNames.at(1);

    Game game = (tracker[gameName])[username];

    std::vector<std::string> statsPrints = game.printGameStats();

    return teamAName + " vs " + teamBName + 
    "\nGame stats:\nGeneral stats:\n" + 
    statsPrints.at(0) + "\n" 
    + teamAName + " stats:\n" 
    + statsPrints.at(1) + "\n" 
    + teamBName + " stats:\n" + 
    statsPrints.at(2) + 
    "\nGame event reports:\n" + 
    game.printEvents();
}

void GameTracker::addGametoTracker(std::string gameName)
{
    tracker[gameName] = boost::unordered_map<std::string, Game>();
}

void GameTracker::removeGameFromTracker(std::string gameName)
{
    tracker.erase(gameName);
}

void GameTracker::addUpdate(std::string username, std::string gameName, boost::unordered_map<std::string,std::string> generalStats, boost::unordered_map<std::string,std::string> teamAStats, boost::unordered_map<std::string,std::string> teamBStats, std::string event)
{
    if((tracker[gameName]).count(username) > 0)
    {
        (tracker[gameName])[username].updateGameStats(generalStats, teamAStats, teamBStats);
    } else 
    {
        (tracker[gameName])[username] = Game(generalStats, teamAStats, teamBStats);
    }

    (tracker[gameName])[username].addEvent(event);
}

void GameTracker::resetGameTracker()
{
    tracker.clear();
}
