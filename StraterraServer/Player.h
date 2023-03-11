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
			BYTE cityBuildingSlots[8];
			BYTE swordLevel;
			BYTE archerLevel;
			BYTE cavalryLevel;
			BYTE spearmanLevel;
			BYTE homeArmy[256];

			int food;
			int wood;
			int metal;
			int order;

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
			std::vector<Straterra::ScheduledEvents::ScheduledEvent*> activeEvents;
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