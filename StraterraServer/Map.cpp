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

#include "Game.h"
#include "EventHub.h"
#include "Player.h"
#include "Map.h"

namespace Straterra
{
	namespace Map
	{
		std::vector<Tile*> map;
		int tileCount = 0;
		void loadMap(std::string path)
		{
			std::ifstream data{ path };
			std::string line;

			while (std::getline(data, line))
			{
				Tile* tile = new Tile();
				
				std::string currentValue = "";
				
				// id
				int index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->id = std::stoi(currentValue);

				// travelCost
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->travelCost = std::stoi(currentValue);

				// tileType
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->tileType = std::stoi(currentValue);

				// building
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->building = std::stoi(currentValue);

				// owner
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->owner = std::stoi(currentValue);

				// foodAmount
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->foodAmount = std::stof(currentValue);

				// woodAmount
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->woodAmount = std::stof(currentValue);

				// metalAmount
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->metalAmount = std::stof(currentValue);

				// chaosAmount
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->chaosAmount = std::stof(currentValue);

				// corruption
				line = line.substr(index + 1);
				index = line.find_first_of(';');
				currentValue = line.substr(0, index);
				std::cout << std::endl << currentValue;
				tile->corruption = std::stof(currentValue);

				map.push_back(tile);
				tileCount++;
			}

			std::cout << "Loaded " << tileCount << " tiles" << std::endl;
		}
		Tile* getTile(int position)
		{
			return map[position];
		}
	}
}