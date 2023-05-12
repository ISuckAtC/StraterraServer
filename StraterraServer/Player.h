#ifndef PLAYER_H
#define PLAYER_H

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
	namespace ScheduledEvents
	{
		class ScheduledEvent;
	}
	namespace Definition
	{
		struct MapBuilding;
	}
	namespace EventHub
	{
		struct Report;
	}
	namespace Player
	{
		class User
		{
		public:
			std::string name;
			std::string login;
			int userId;
			int color;
			int allianceId;
			int cityLocation;
			int path;

			BYTE cityUpgradeCap;

			BYTE cityBuildingSlots[8];
			BYTE swordLevel;
			BYTE archerLevel;
			BYTE cavalryLevel;
			BYTE spearmanLevel;
			BYTE homeArmy[256];

			bool unitUpgrading;

			int food;
			int wood;
			int metal;
			int order;

			int foodMax;
			int woodMax;
			int metalMax;
			int orderMax;

			int foodGeneration;
			float foodMultiplier;
			int restFood;
			int woodGeneration;
			float woodMultiplier;
			int restWood;
			int metalGeneration;
			float metalMultiplier;
			int restMetal;
			int orderGeneration;
			float orderMultiplier;
			int restOrder;

			int population;
			int populationCap;

			std::vector<Straterra::ScheduledEvents::ScheduledEvent*> activeEvents;
			std::vector<EventHub::Report*> reports;
			std::vector<int> mapBuildings;
			~User();
			User();

			int getFoodTickValue();
			int getWoodTickValue();
			int getMetalTickValue();
			int getOrderTickValue();

			void addResources();
			std::string getResourcesJson(long long token);

		private:

		};
	}
}


#endif