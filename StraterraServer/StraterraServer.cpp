// StraterraServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>
#include "EventHub.h"
#include "Http.h"
#include "Definition.h"
#include "Game.h"
#include "Player.h"
#include "Map.h"
#include "ScheduledEvent.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace Straterra;

static bool debug = false;


int main(int argc, char** argv)
{
	Straterra::Definition::DefineUnits();
	Straterra::Definition::DefineMapBuildings();
	Straterra::Definition::DefineTownBuildings();
	Straterra::Definition::DefineInternalUpgrades();
	/*
	std::vector<Group*> groupA;
	std::vector<Group*> groupB;

	Group archersA = { 20, 0 };
	Group swordsA = { 20, 2 };

	Group swordsB = { 20, 2 };
	Group archersB = { 20, 0 };

	std::vector<Group*>::iterator it;
	it = groupA.begin();
	groupA.insert(it, &archersA);
	it = groupA.begin();
	groupA.insert(it, &swordsA);

	it = groupB.begin();
	groupB.insert(it, &swordsB);
	it = groupB.begin();
	groupB.insert(it, &archersB);

	std::string output;
	std::vector<Group> remains;
	Fight(&remains, &output, groupA, groupB, true);

	std::cout << output << std::endl;
	*/
	std::cout << "Argument list:" << std::endl;
	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
	}
	std::cout << std::endl;
	
	if (!debug && argc != 6)
	{
		std::cout << "Argument count invalid, excepted 6, is " << std::to_string(argc) << std::endl;
		return 1;
	}

	try
	{

		Straterra::Map::loadMap(debug ? "C:/Users/Student/Desktop/sjfs/MapInformation.txt" : argv[3]);

		std::ifstream lastStartIndexFile{ argv[4] };
		std::stringstream ssLSI;
		ssLSI << lastStartIndexFile.rdbuf();

		int lastStartIndex = std::stoi(ssLSI.str());
		Game::setLastStartIndex(lastStartIndex);
		std::cout << "Set last start index to " << lastStartIndex << std::endl;

		{
			std::ifstream data{ argv[5] };
			std::string line;

			while (std::getline(data, line, ';'))
			{
				int startLocation = std::stoi(line);
				Game::addStartLocation(startLocation);
				std::cout << "Added startLocation: " << startLocation << std::endl;
			}
		}


		Straterra::Game::start(debug ? 1000 : std::stoi(argv[1]), 10);

	}
	catch (const std::exception& e)
	{
		std::wcout << e.what() << std::endl;
		return 1;
	}

	int lines = 0;
	try
	{
		std::ifstream data{ debug ? "C:/Users/Student/Desktop/sjfs/players.txt" : argv[2]};
		std::string line;

		std::cout << std::endl << "Loading users: ";
		while (std::getline(data, line))
		{

			// userId
			int index = line.find_first_of(';');
			std::string currentValue = "";
			Player::User* u = new Player::User();
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->userId = std::stoi(line.substr(0, index));


			// login
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->login = line.substr(0, index);


			// name
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->name = line.substr(0, index);


			// cityLocation
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->cityLocation = std::stoi(line.substr(0, index));


			// color
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->color = std::stoi(line.substr(0, index));


			// allianceId
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->allianceId = std::stoi(line.substr(0, index));

			// food
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->food = std::stoi(line.substr(0, index));

			// wood
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->wood = std::stoi(line.substr(0, index));

			// metal
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->metal = std::stoi(line.substr(0, index));

			// order
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->order = std::stoi(line.substr(0, index));

			// foodGeneration
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->foodGeneration = std::stoi(line.substr(0, index));

			// woodGeneration
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->woodGeneration = std::stoi(line.substr(0, index));

			// metalGeneration
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->metalGeneration = std::stoi(line.substr(0, index));

			// orderGeneration
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			currentValue = line.substr(0, index);
			//std::cout << std::endl << currentValue;
			u->orderGeneration = std::stoi(line.substr(0, index));

			// cityBuildingSlots
			for (int i = 0; i < 8; ++i)
			{
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				//std::cout << std::endl << currentValue;
				u->cityBuildingSlots[i] = std::stoi(line.substr(0, index));
			}

			// Unit levels
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			u->archerLevel = std::stoi(line.substr(0, index));

			line = line.substr(index + 1);
			index = line.find_first_of(';');
			u->cavalryLevel = std::stoi(line.substr(0, index));

			line = line.substr(index + 1);
			index = line.find_first_of(';');
			u->swordLevel = std::stoi(line.substr(0, index));

			line = line.substr(index + 1);
			index = line.find_first_of(';');
			u->spearmanLevel = std::stoi(line.substr(0, index));

			// City Upgrade Cap
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			u->cityUpgradeCap = std::stoi(line.substr(0, index));

			// Population and Population Cap
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			u->populationCap = std::stoi(line.substr(0, index));

			line = line.substr(index + 1);
			index = line.find_first_of(';');
			u->population = std::stoi(line.substr(0, index));


			if (u->userId == 109)
			{
				std::cout << "CHECKING USER 109" << std::endl;

				std::cout << "ARCHERLEVEL: " << std::to_string(u->archerLevel) << std::endl;
				std::cout << "CAVALRYLEVEL: " << std::to_string(u->cavalryLevel) << std::endl;
				std::cout << "SWORDLEVEL: " << std::to_string(u->swordLevel) << std::endl;
				std::cout << "SPEARMANLEVEL: " << std::to_string(u->spearmanLevel) << std::endl;
			}


			u->foodMultiplier = 1.;
			u->woodMultiplier = 1.;
			u->metalMultiplier = 1.;
			u->orderMultiplier = 1.;

			std::cout << "Added user: " << (long)u << " id: " << u->userId << std::endl;
			Game::addUser(u);
		}
		std::cout << std::endl << "total users: " << std::to_string(Game::getUserCount()) << std::endl;
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	//Game::ScheduledEvent sEvent{ 60, 69, true };

	try
	{
		//ScheduledEvents::ScheduledUnitProductionEvent ae{ 10, 0, 3, 69 };

		//std::cout << "JASHFJHSAF: " << ae.secondsLeft << std::endl;

		Map::Tile* testTile = Map::getTile(8888);
		std::cout << "TESTTILE: " << testTile->id << " | " <<
			testTile->owner << " | " <<
			std::to_string(testTile->building) << " | " <<
			testTile->travelCost << " | " <<
			testTile->foodAmount << std::endl;

		auto const address = net::ip::make_address_v4("0.0.0.0");
		unsigned short port = 80;

		net::io_context ioc{ 1 };

		Straterra::Http::http_connection a{ tcp::socket{ioc} };

		tcp::acceptor acceptor{ ioc, {address, port} };
		tcp::socket socket{ ioc };

		Straterra::Http::http_server(acceptor, socket);

		ioc.run();

		std::cout << "Finished startup, entering ioc watch" << std::endl;

		while (true)
		{
			std::cout << "ioc ran out of work?" << std::endl;
			ioc.restart();
			ioc.run();
		}

		//return 0;
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
		//return 1;
	}
	std::cout << "End of program" << std::endl;
	return 0;
}

