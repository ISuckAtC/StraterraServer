#pragma once
#ifndef MAP_H
#define MAP_H

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

namespace Straterra
{
	namespace Game
	{
		class Group;
	}
	namespace Map
	{
		struct Tile
		{
			int id;
			int travelCost;
			BYTE tileType;
			BYTE building;
			int owner;
			float foodAmount;
			float woodAmount;
			float metalAmount;
			float chaosAmount;
			float corruption;
			std::vector<Game::Group*> army;
		};

		Tile* getTile(int position);
		void loadMap(std::string path);
	}
}

#endif