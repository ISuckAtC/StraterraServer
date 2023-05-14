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

#include "Game.h"
#include "EventHub.h"
#include "Player.h"

using namespace Straterra::Game;

namespace Straterra
{
	namespace Player
	{
		User userFromFile()
		{

		}
		User::User()
		{
			this->foodMax = 1024;
			this->woodMax = 1024;
			this->metalMax = 1024;
			this->order = 100;
			this->unitUpgrading = false;
		}
		int User::getFoodTickValue()
		{
			restFood += (int)(foodGeneration * foodMultiplier);
			int food = restFood / getTicksPerHour();
			restFood = restFood % getTicksPerHour();

			return food;
		}
		int User::getWoodTickValue()
		{
			restWood += (int)(woodGeneration * foodMultiplier);
			int wood = restWood / getTicksPerHour();
			restWood = restWood % getTicksPerHour();

			return wood;
		}
		int User::getMetalTickValue()
		{
			restMetal += (int)(metalGeneration * metalMultiplier);
			int metal = restMetal / getTicksPerHour();
			restMetal = restMetal % getTicksPerHour();

			return metal;
		}
		int User::getOrderTickValue()
		{
			restOrder += (int)(orderGeneration * orderMultiplier);
			int order = restOrder / getTicksPerHour();
			restOrder = restOrder % getTicksPerHour();

			return order;
		}

		void User::addResources()
		{
			try{
			food += getFoodTickValue();
			wood += getWoodTickValue();
			metal += getMetalTickValue();
			order += getOrderTickValue();

			if (food > foodMax) food = foodMax;
			if (wood > woodMax) wood = woodMax;
			if (metal > metalMax) metal = metalMax;
			if (order > orderMax) order = orderMax;

			//if (userId == 69) std::cout << std::to_string((long)this) << "|" << food << std::endl;
			//std::cout << "Resources added for user " << userId << " totals are now: " <<
			//	std::to_string(food) << " | " <<
			//	std::to_string(wood) << " | " <<
			//	std::to_string(metal) << " | " <<
			//	std::to_string(order) << " | " <<
			//	std::endl;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}

		User::~User()
		{
			std::cout << "User was destroyed, p" << std::to_string((long)this) << std::endl;
		}
		
	}
}