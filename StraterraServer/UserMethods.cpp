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
#include <iomanip>
#include <boost/signals2/signal.hpp>
#include <boost/bind/bind.hpp>
#include <boost/algorithm/string.hpp>

#include "Game.h"
#include "EventHub.h"
#include "Player.h"
#include "Definition.h"
#include "ScheduledEvent.h"
#include "Map.h"

using namespace Straterra::Game;
using namespace Straterra::Player;
using namespace Straterra::Definition;
using namespace Straterra::ScheduledEvents;

namespace Straterra
{
	namespace UserMethods
	{
		void logout(long long token, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				removeSession(user->userId);
			}
			catch (const std::exception& e)
			{

			}
		}
		void stationArmy(long long token, int position, std::string units, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == - 1)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				Map::Tile* mapTile = Map::getTile(position);

				if (mapTile->owner != user->userId)
				{
					*out = "{\"success\":\"false\",\"message\":\"Can't station army on a tile you do not own\"}";
					*code = 2;
					return;
				}

				if (mapTile->building == 1)
				{
					*out = "{\"success\":\"false\",\"message\":\"Can't station army in cities\"}";
					*code = 2;
					return;
				}

				std::vector<std::string> unitSplit;
				boost::split(unitSplit, units, boost::is_any_of(";"));

				if (unitSplit.size() <= 0)
				{
					std::cout << "tried to sation with no units" << std::endl;
					*out = "{\"success\":\"false\",\"message\":\"Tried to station with no units\"}";
					*code = 2;
					return;
				}

				std::vector<Group> army;

				for (int i = 0; i < unitSplit.size(); ++i)
				{
					int colonIndex = unitSplit[i].find_first_of(':');
					int unitId = std::stoi(unitSplit[i].substr(0, colonIndex));
					int amount = std::stoi(unitSplit[i].substr(colonIndex + 1));

					if (amount > user->homeArmy[unitId])
					{
						std::cout << "tried to attack with units they do not have" << std::endl;
						*out = "{\"success\":\"false\",\"message\":\"Tried to attack with units they do not have\"}";
						*code = 2;
						return;
					}

					army.push_back(Group(amount, unitId));
				}

				for (int i = 0; i < army.size(); ++i)
				{
					Group g = army[i];
					user->homeArmy[g.unitId] -= g.count;
				}

				new ScheduledMoveArmyEvent(20, army, position, user->cityLocation, user->userId);

				*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
				*code = 3;
			}
			catch (const std::exception& e)
			{

			}
		}
		void recallArmy(long long token, int position, std::string units, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				Map::Tile* mapTile = Map::getTile(position);

				if (mapTile->owner != user->userId)
				{
					*out = "{\"success\":\"false\",\"message\":\"Can't recall from a tile you don't own\"}";
					*code = 2;
					return;
				}

				if (mapTile->army.size() == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"No units to recall\"}";
					*code = 2;
					return;
				}

				std::vector<std::string> unitSplit;
				boost::split(unitSplit, units, boost::is_any_of(";"));

				if (unitSplit.size() <= 0)
				{
					std::cout << "tried to recall with no units" << std::endl;
					*out = "{\"success\":\"false\",\"message\":\"Tried to recall with no units\"}";
					*code = 2;
					return;
				}

				std::vector<Group> parsedArmy;

				for (int i = 0; i < unitSplit.size(); ++i)
				{
					int colonIndex = unitSplit[i].find_first_of(':');
					int unitId = std::stoi(unitSplit[i].substr(0, colonIndex));
					int amount = std::stoi(unitSplit[i].substr(colonIndex + 1));

					int tileAmount = 0;
					int tileIndex = -1;
					for (int k = 0; k < mapTile->army.size(); ++k)
					{
						if (mapTile->army[k].unitId == unitId)
						{
							tileIndex = k;
							tileAmount = mapTile->army[k].count;
						}
					}

					if (amount > tileAmount)
					{
						std::cout << "tried to recall with units they do not have" << std::endl;
						*out = "{\"success\":\"false\",\"message\":\"Tried to recall with units they do not have\"}";
						*code = 2;
						return;
					}

					if (amount == tileAmount)
					{
						mapTile->army.erase(mapTile->army.begin() + tileIndex);
					}
					else
					{
						mapTile->army[tileIndex].count -= amount;
					}

					parsedArmy.push_back(Group(amount, unitId));
				}

				

				new ScheduledMoveArmyEvent(20, parsedArmy, user->cityLocation, position, user->userId);

				*out = "{\"success\":\"false\",\"message\":\"No units to recall\"}";
				*code = 2;
				return;
			}
			catch (const std::exception& e)
			{

			}
		}
		void upgradeStorage(long long token, int upgradeIndex, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				StorageUpgrade upgrade = *(StorageUpgrade*)Definition::getInternalUpgradeDefinition(upgradeIndex);


				// Check if user has enough resources
				if (upgrade.foodCost > user->food ||
					upgrade.woodCost > user->wood ||
					upgrade.metalCost > user->metal ||
					upgrade.orderCost > user->order)
				{
					*out = "{\"success\":\"false\",\"message\":\"Not enough resources\"}";
					*code = 3;
					return;
				}

				// Pay
				user->food -= upgrade.foodCost;
				user->wood -= upgrade.woodCost;
				user->metal -= upgrade.metalCost;
				user->order -= upgrade.orderCost;


				if (upgrade.foodMax > -1) user->foodMax = upgrade.foodMax;
				if (upgrade.woodMax > -1) user->woodMax = upgrade.woodMax;
				if (upgrade.metalMax > -1) user->metalMax = upgrade.metalMax;
				if (upgrade.orderMax > -1) user->orderMax = upgrade.orderMax;

				//TODO: make scheduledevent for it

				new ScheduledInternalUpgradeEvent(upgrade.upgradeTime, upgradeIndex, user->userId);

				*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
				*code = 3;
			}
			catch (const std::exception& e)
			{

			}
		}
		void removeAllNotifications(long long token, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				user->reports.clear();

				*code = 3;
				*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
			}
			catch (const std::exception& e)
			{

			}
		}
		void removeNotification(long long token, int reportIndex, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				if (user->reports.size() <= reportIndex)
				{
					*out = "{\"success\":\"false\",\"message\":\"Invalid report index\"}";
					*code = 2;
					return;
				}

				user->reports.erase(user->reports.begin() + reportIndex);

				*code = 3;
				*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
			}
			catch (const std::exception& e)
			{

			}
		}
		void viewNotification(long long token, int reportIndex, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				if (user->reports.size() <= reportIndex)
				{
					*out = "{\"success\":\"false\",\"message\":\"Invalid report index\"}";
					*code = 2;
					return;
				}

				user->reports[reportIndex]->viewed = true;

				*code = 3;
				*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
			}
			catch (const std::exception& e)
			{

			}
		}
		void getNotifications(long long token, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				std::stringstream reports;

				reports << "{\"reports\":[";

				for (int i = 0; i < user->reports.size(); ++i)
				{
					if (i > 0) reports << ",";
					reports <<
						"{" <<
						"\"title\":\"" << user->reports[i]->title << "\"," <<
						"\"content\":\"" << user->reports[i]->content << "\"," <<
						"\"time_created\":\"" << user->reports[i]->timeCreated << "\"," <<
						"\"viewed\":\"" << user->reports[i]->viewed << "\"" <<
						"}";
				}

				reports << "]}";

				*code = 3;
				*out = reports.str();
			}
			catch (const std::exception& e)
			{

			}
		}
		void getUpdate(long long token, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				int notificationCount = 0;

				for (int i = 0; i < user->reports.size(); ++i)
				{
					if (!user->reports[i]->viewed) notificationCount++;
				}

				std::ostringstream oss;
				oss << 
					"{\"resources\":{" <<
						"\"food\":\"" << std::to_string(user->food) << "\"," <<
						"\"wood\":\"" << std::to_string(user->wood) << "\"," <<
						"\"metal\":\"" << std::to_string(user->metal) << "\"," <<
						"\"order\":\"" << std::to_string(user->order) << "\"" <<
					"}" << 
					",\"notifications\":\"" << notificationCount << "\"" <<
					"}";
				*out = oss.str();
				*code = 0;
			}
			catch (const std::exception& e)
			{

			}
		}
		void setUserColor(long long token, int color, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				user->color = color;

				*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
				*code = 3;
			}
			catch (const std::exception& e)
			{

			}
		}
		
		void upgradeUnit(long long token, int unitId, std::string* out, int* code)
		{
			try
			{
			// Grab and verify user
			User* user = getUserBySession(token);
			if ((long)user == 0)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			if (user->unitUpgrading)
			{
				*out = "{\"success\":\"false\",\"message\":\"Already upgrading a unit\"}";
				*code = 2;
				return;
			}

			int unitType = unitId / 10;

			switch (unitType)
			{
				// Archer
			case 0:
			{
				if (user->archerLevel != unitId - 1)
				{
					// Invalid upgrade
					*out = "{\"success\":\"false\",\"message\":\"Upgrade invalid\"}";
					*code = 2;
					return;
				}
				break;
			}
			// Cavalry
			case 1:
			{
				if (user->cavalryLevel != unitId - 1)
				{
					// Invalid upgrade
					*out = "{\"success\":\"false\",\"message\":\"Upgrade invalid\"}";
					*code = 2;
					return;
				}
				break;
			}
			// Swordsman
			case 2:
			{
				if (user->swordLevel != unitId - 1)
				{
					// Invalid upgrade
					*out = "{\"success\":\"false\",\"message\":\"Upgrade invalid\"}";
					*code = 2;
					return;
				}
				break;
			}
			// Spearman
			case 3:
			{
				if (user->spearmanLevel != unitId - 1)
				{
					// Invalid upgrade
					*out = "{\"success\":\"false\",\"message\":\"Upgrade invalid\"}";
					*code = 2;
					return;
				}
				break;
			}
			}

			Definition::Unit unit = Definition::getUnitDefinition(unitId);

			// Check if user has enough resources
			if (unit.upgradeFoodCost > user->food ||
				unit.upgradeWoodCost > user->wood ||
				unit.upgradeMetalCost > user->metal ||
				unit.upgradeOrderCost > user->order)
			{
				*out = "{\"success\":\"false\",\"message\":\"Not enough resources\"}";
				*code = 3;
				return;
			}

			// Pay
			user->food -= unit.upgradeFoodCost;
			user->wood -= unit.upgradeWoodCost;
			user->metal -= unit.upgradeMetalCost;
			user->order -= unit.upgradeOrderCost;

			user->unitUpgrading = true;
			new ScheduledUnitUpgradeEvent(unit.upgradeTime, unit.id, user->userId);
			*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
			*code = 3;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void choosePath(long long token, int choice, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				if (user->path != 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Path already chosen\"}";
					*code = 2;
					return;
				}

				if (choice > 4)
				{
					*out = "{\"success\":\"false\",\"message\":\"Path doesn't exist\"}";
					*code = 2;
					return;
				}

				user->path = choice;

				*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
				*code = 3;
			}
			catch (const std::exception& e)
			{

			}
		}
		void attackMapTile(long long token, int destination, std::string units, std::string* out, int* code)
		{
			std::cout << destination << std::endl;
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if ((long)user == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				Map::Tile* mapTile = Map::getTile(destination);

				if (mapTile->building == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Nothing to attack\"}";
					*code = 2;
					return;
				}
				if (mapTile->owner == user->userId)
				{
					*out = "{\"success\":\"false\",\"message\":\"You cannot attack your own tiles\"}";
					*code = 2;
					return;
				}

				std::vector<std::string> unitSplit;
				boost::split(unitSplit, units, boost::is_any_of(";"));

				if (unitSplit.size() <= 0)
				{
					std::cout << "tried to attack with no units" << std::endl;
					*out = "{\"success\":\"false\",\"message\":\"Tried to attack with no units\"}";
					*code = 2;
					return;
				}

				std::vector<Group> army;

				for (int i = 0; i < unitSplit.size(); ++i)
				{
					int colonIndex = unitSplit[i].find_first_of(':');
					int unitId = std::stoi(unitSplit[i].substr(0, colonIndex));
					int amount = std::stoi(unitSplit[i].substr(colonIndex + 1));

					if (amount > user->homeArmy[unitId])
					{
						std::cout << "tried to attack with units they do not have" << std::endl;
						*out = "{\"success\":\"false\",\"message\":\"Tried to attack with units they do not have\"}";
						*code = 2;
						return;
					}

					army.push_back(Group(amount, unitId));
				}

				for (int i = 0; i < army.size(); ++i)
				{
					Group g = army[i];
					user->homeArmy[g.unitId] -= g.count;
				}

				new ScheduledAttackEvent(20, army, destination, user->cityLocation, user->userId);

				*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
				*code = 3;
			}
			catch (const std::exception& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void getMapTile(long long token, int position, std::string* out, int* code)
		{
			try{
			// Grab and verify user
			User* user = getUserBySession(token);
			if ((long)user == 0)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			Map::Tile* tile = Map::getTile(position);
			
			std::ostringstream oss;
			oss << "{" <<
				"\"travelCost\":\"" << std::to_string(tile->travelCost) << "\"," <<
				"\"building\":\"" << std::to_string(tile->building) << "\"," <<
				"\"owner\":\"" << std::to_string(tile->owner) << "\"," <<
				"\"foodAmount\":\"" << std::to_string(tile->foodAmount) << "\"," <<
				"\"woodAmount\":\"" << std::to_string(tile->woodAmount) << "\"," <<
				"\"metalAmount\":\"" << std::to_string(tile->metalAmount) << "\"," <<
				"\"corruptionProgress\":\"" << std::to_string(tile->corruption) << "\"," <<
				"\"army\":[";

			for (int i = 0; i < tile->army.size(); ++i)
			{
				if (i > 0) oss << ",";
				oss << "{" <<
					"\"unitId\":\"" << std::to_string(tile->army[i].unitId) << "\"," <<
					"\"amount\":\"" << std::to_string(tile->army[i].count) << "\"" <<
					"}";
			}

			oss << "]}";

			*out = oss.str();
			*code = 0;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void getHomeUnits(long long token, std::string* out, int* code)
		{
			try{
			// Grab and verify user
			User* user = getUserBySession(token);
			if ((long)user == 0)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			std::ostringstream oss;
			oss << "{\"units\":[";
			int actualUnits = 0;
			for (int i = 0; i < 256; ++i)
			{
				int amount = user->homeArmy[i];
				if (amount > 0)
				{
					// If the unit of ID i is present we add it to the list
					if (actualUnits > 0) oss << ",";
					actualUnits++;
					oss << "{\"unitId\":\"" << i << "\"," <<
						"\"amount\":\"" << amount << "\"";
					oss << "}";
				}
			}
			oss << "]}";

			std::cout << actualUnits << " unit groups returned" << std::endl;

			*out = oss.str();
			*code = 0;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void getScheduledEvents(long long token, std::string* out, int* code)
		{
			try{
			// Grab and verify user
			User* user = getUserBySession(token);
			if ((long)user == 0)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			std::ostringstream oss;
			oss << "{\"events\":[";

			for (int i = 0; i < user->activeEvents.size(); ++i)
			{
				oss << "{\"secondsLeft\":\"" << user->activeEvents[i]->secondsLeft << "\"," <<
					"\"type\":\"" << user->activeEvents[i]->type << "\"," <<
					"\"owner\":\"" << user->activeEvents[i]->owner << "\"," <<
					"\"running\":\"" << user->activeEvents[i]->running << "\"";
				
				// Add extras based on which type of event it is
				switch (user->activeEvents[i]->type)
				{
				case UNITPRODUCTION:
				{
					ScheduledUnitProductionEvent* uProdEvent = (ScheduledUnitProductionEvent*)(user->activeEvents[i]);
					oss << ",\"unitId\":\"" << uProdEvent->unitId << "\"," <<
						"\"amount\":\"" << uProdEvent->amount << "\"";
					break;
				}
				case TOWNBUILDING:
				{
					ScheduledTownBuildingEvent* bEvent = (ScheduledTownBuildingEvent*)(user->activeEvents[i]);
					oss << ",\"buildingId\":\"" << bEvent->buildingId << "\"," <<
						"\"buildingSlot\":\"" << bEvent->buildingSlot << "\"";
					break;
				}
				case MAPBUILDING:
				{
					ScheduledMapBuildingEvent* mEvent = (ScheduledMapBuildingEvent*)(user->activeEvents[i]);
					oss << ",\"buildingId\":\"" << mEvent->buildingId << "\"," <<
						"\"position\":\"" << mEvent->position << "\"";
					break;
				}
				case MOVEARMY:
				{
					ScheduledMoveArmyEvent* maEvent = (ScheduledMoveArmyEvent*)(user->activeEvents[i]);
					oss << ",\"destination\":\"" << maEvent->destination << "\"," <<
						"\"origin\":\"" << maEvent->origin << "\"";
				}
				case ATTACK:
				{
					ScheduledAttackEvent* aEvent = (ScheduledAttackEvent*)(user->activeEvents[i]);
					oss << ",\"destination\":\"" << aEvent->destination << "\"," <<
						"\"origin\":\"" << aEvent->origin << "\"";
				}
				case UNITUPGRADE:
				{
					ScheduledUnitUpgradeEvent* uUpgradeEvent = (ScheduledUnitUpgradeEvent*)(user->activeEvents[i]);
					oss << ",\"unitId\":\"" << uUpgradeEvent->unitId << "\"";
				}
				}
				oss << "}";
				if (i < user->activeEvents.size() - 1) oss << ",";
			}
			oss << "]}";

			*out = oss.str();
			*code = 0;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void createMapBuilding(long long token, int buildingId, int position, std::string* out, int* code)
		{
			try{
			// Grab the current user by session token
			User* user = getUserBySession(token);

			// Verify that they are connected
			if ((long)user == 0)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			// Check if something is already built here
			if (Map::getTile(position)->building != 0)
			{
				*out = "{\"success\":\"false\",\"message\":\"Can't build here\"}";
				*code = 4;
				return;
			}


			// Grab definition
			MapBuilding mapBuilding = getMapBuildingDefinition(buildingId);


			// Check if user has enough resources
			if (mapBuilding.foodCost > user->food ||
				mapBuilding.woodCost > user->wood ||
				mapBuilding.metalCost > user->metal ||
				mapBuilding.orderCost > user->order)
			{
				*out = "{\"success\":\"false\",\"message\":\"Not enough resources\"}";
				*code = 3;
				return;
			}

			// Pay
			user->food -= mapBuilding.foodCost;
			user->wood -= mapBuilding.woodCost;
			user->metal -= mapBuilding.metalCost;
			user->order -= mapBuilding.orderCost;

			// TODO
			// Check if current building has any production value
			// then remove it from the player (upgrading a farm for example)

			// Use new to prevent the object from being destructed
			// We destroy it manually when the event completes
			new ScheduledMapBuildingEvent{ mapBuilding.buildingTime / 5, mapBuilding.id, position, user->userId };

			*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
			*code = 3;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void createUnits(long long token, int unitId, int amount, std::string* out, int* code)
		{
			try{
			// Grab the current user by session token
			User* user = getUserBySession(token);

			// Verify that they are connected
			if ((long)user == 0)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			// Grab the definition for the desired unit
			Unit unit = getUnitDefinition(unitId);

			if (amount < 1)
			{
				*out = "{\"success\":\"false\",\"message\":\"You must create at least one unit\"}";
				*code = 3;
				return;
			}

			// Check if the user has the population room to train the troops
			if (user->population + amount > user->populationCap)
			{
				*out = "{\"success\":\"false\",\"message\":\"Too much population\"}";
				*code = 3;
				return;
			}

			// Get total costs
			int foodCost = amount * unit.foodCost;
			int woodCost = amount * unit.woodCost;
			int metalCost = amount * unit.metalCost;
			int orderCost = amount * unit.orderCost;

			// Check if the user has the resources to train the troops
			if (foodCost > user->food ||
				woodCost > user->wood ||
				metalCost > user->metal ||
				orderCost > user->order)
			{
				std::cout <<
					foodCost << " | " << user->food << std::endl <<
					woodCost << " | " << user->wood << std::endl <<
					metalCost << " | " << user->metal << std::endl <<
					orderCost << " | " << user->order << std::endl;
				*out = "{\"success\":\"false\",\"message\":\"Not enough resources\"}";
				*code = 3;
				return;
			}

			//std::cout << "UNITCREATION: unitTrainingTime: " << unit.trainingTime << " | amount: " << amount << std::endl;

			// Pay
			user->food -= foodCost;
			user->wood -= woodCost;
			user->metal -= metalCost;
			user->order -= orderCost;

			user->population += amount;

			// Use new to prevent the object from being destructed
			// We destroy it manually when the event completes
			new ScheduledUnitProductionEvent{ unit.trainingTime, unit.id, amount, user->userId };

			*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
			*code = 3;

			//std::cout << "methodDONEDONEDONE" << std::endl;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void createTownBuilding(long long token, int buildingId, int buildingSlot, std::string* out, int* code)
		{
			try{
			// Grab the current user by session token
			User* user = getUserBySession(token);
			
			// Verify that they are connected
			if ((long)user == 0)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			// Grab the definition for the desired building
			TownBuilding townBuilding = getTownBuildingDefinition(buildingId);

			int currentBuilding = user->cityBuildingSlots[buildingSlot];

			TownBuilding prevBuilding = getTownBuildingDefinition(currentBuilding);

			// Check if there is already something built in this slot
			// It can still go through if the ID is 1 lower (upgrading)
			if (currentBuilding != 255)
			{
				if (currentBuilding == buildingId - 1)
				{
					// If the level of the next building isnt above, something is wrong
					if (townBuilding.level <= prevBuilding.level)
					{
						*out = "{\"success\":\"false\",\"message\":\"Can't build here (level mismatch)\"}";
						*code = 4;
						return;
					}
				}
				else
				{
					*out = "{\"success\":\"false\",\"message\":\"Can't build here (not empty plot)\"}";
					*code = 4;
					return;
				}
			}

			// Check if the user has high enough TownHall level to build this building
			if (townBuilding.type != Definition::TOWNHALL)
			{
				if (townBuilding.level > user->cityUpgradeCap)
				{
					*out = "{\"success\":\"false\",\"message\":\"Town Hall level not high enough\"}";
					*code = 3;
					return;
				}
			}
			else
			{
				if (townBuilding.level == 1 && user->cityUpgradeCap > 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Cannot build town hall, you already have one\"}";
					*code = 3;
					return;
				}
			}
			

			// Check if the user has the resources to build this building
			if (townBuilding.foodCost > user->food ||
				townBuilding.woodCost > user->wood ||
				townBuilding.metalCost > user->metal ||
				townBuilding.orderCost > user->order)
			{
				std::cout << townBuilding.foodCost << " | " <<
					townBuilding.woodCost << " | " <<
					townBuilding.metalCost << " | " <<
					townBuilding.orderCost << " | " << std::endl;
				*out = "{\"success\":\"false\",\"message\":\"Not enough resources\"}";
				*code = 3;
				return;
			}

			// Pay for the building
			user->food -= townBuilding.foodCost;
			user->wood -= townBuilding.woodCost;
			user->metal -= townBuilding.metalCost;
			user->order -= townBuilding.orderCost;

			// Use new to prevent the object from being destructed
			// We destroy it manually when the event completes
			new ScheduledTownBuildingEvent{ townBuilding.buildingTime, townBuilding.id, buildingSlot, user->userId };

			*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
			*code = 3;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}

		void createUser(std::string email, std::string name, std::string loginInfo, std::string* out, int* code)
		{
			try{
			std::cout << "[createUser] name: \"" << name << "\" login: \"" << loginInfo << "\"" << std::endl;
			Player::User* u = new Player::User();
			u->allianceId = 0;
			u->name = name;
			u->login = loginInfo;
			u->color = 0;
			u->cityLocation = Game::getNextStartLocation();
			for (int i = 0; i < 8; ++i) u->cityBuildingSlots[i] = 255;
			u->userId = Game::createUserId();

			u->food = 1000;
			u->wood = 1000;
			u->metal = 500;
			u->order = 0;

			u->foodGeneration = 0;
			u->woodGeneration = 0;
			u->metalGeneration = 0;
			u->orderGeneration = 0;
			u->restFood = 0;
			u->restWood = 0;
			u->restMetal = 0;
			u->restOrder = 0;
			u->foodMultiplier = 1.;
			u->woodMultiplier = 1.;
			u->metalMultiplier = 1.;
			u->orderMultiplier = 1.;

			addUser(u);
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void login(std::string* out, int* code, std::string loginInfo)
		{
			try{

			std::cout << "Users total: " << getUserCount() << std::endl;
			for (int i = 0; i < getUserCount(); ++i)
			{
				Player::User* u = getUserAt(i);
				//std::cout << std::to_string((long)u) << " | ";
				std::cout << "name: \"" << u->name << "\" | login: \"" << u->login << "\"" << std::endl;
				try
				{
					//std::cout << "user login: " << u->login << " | provided: " << loginInfo << std::endl;
					//std::cout << "==: " << (u->login == loginInfo) << " | compare: " << (loginInfo.compare(u->login)) << std::endl;
					if (u->login == loginInfo)
					{
						if (getUserOnline(u->userId))
						{
							*code = 0;
							*out = "{\"success\":\"false\",\"message\":\"This user is already logged in!\"}";
							return;
						}

						long long token = createSessionToken();
						std::cout << token << std::endl;
						Session* s = new Session(u->userId, token);
						addSession(s);

						//std::cout << "session token created: " << s->token << " (" << token << ")" << std::endl;
						//std::cout << "token string: " << getTokenString(token) << std::endl;
						//std::cout << "token string to long again: " << getTokenLong(getTokenString(token)) << std::endl;


						std::cout << "Users online (including you): " << getUserOnlineCount() << std::endl;
						*code = 0;
						*out = "{\"success\":\"true\",\"message\":\"" + getTokenString(token) + "\"}";
						return;
					}
					//std::cout << "b" << std::endl;
				}
				catch (std::exception const& e)
				{
					std::cerr << "ERROR: " << e.what() << std::endl;
				}
			}

			*code = 2;
			*out = "{\"success\":\"false\",\"message\":\"No player with that login\"}";
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void getResources(long long token, int userId, std::string* out, int* code)
		{
			try{
			int id = findUserBySession(token);
			if (id == -1)
			{
				*code = 2;
				*out = "{\"error\":\"Session invalid\"}";
				return;
			}
			User* user = getUserById(userId);
			//std::cout << "getResources VALUE: id: " << std::to_string((long)user) << "|" << std::to_string(user->userId) << "|" << std::to_string(user->food) << std::endl;
			if ((long)user == 0)
			{
				*out = "{\"error\":\"No user with that id\"}";
				*code = 2;
				return;
			}
			std::ostringstream oss;
			oss << "{" <<
				"\"food\":\"" << std::to_string(user->food) << "\"," <<
				"\"wood\":\"" << std::to_string(user->wood) << "\"," <<
				"\"metal\":\"" << std::to_string(user->metal) << "\"," <<
				"\"order\":\"" << std::to_string(user->order) << "\"" <<
				"}";
			*out = oss.str();
			*code = 0;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void getPlayers(long long token, std::string* out, int* code)
		{
			try{
			int id = findUserBySession(token);
			if (id == -1)
			{
				*code = 2;
				*out = "{\"error\":\"Session invalid\"}";
				return;
			}

			std::ostringstream oss;
			oss << "{\"players\":[";
			for (int i = 0; i < getUserCount(); ++i)
			{
				User* user = getUserAt(i);
				oss << "{" << 
					"\"userId\":\"" << std::to_string(user->userId) << "\"," << 
					"\"name\":\"" << user->name << "\"," << 
					"\"cityLocation\":\"" << std::to_string(user->cityLocation) << "\"," << 
					"\"path\":\"" << std::to_string(user->path) << "\"," << 
					"\"buildingPositions\":[";
				for (int k = 0; k < user->mapBuildings.size(); ++k)
				{
					if (k > 0) oss << ",";
					int position = user->mapBuildings[k];
					oss << "{\"building\":\"" << std::to_string(Map::getTile(position)->building) << "\","
						<< "\"position\":\"" << position << "\"}";
				}
				oss << "]}";
				if (i < getUserCount() - 1) oss << ",";
			}
			oss << "]}";
			*out = oss.str();
			*code = 0;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void getUser(long long token, int userId, std::string* out, int* code)
		{
			try{
			int id = findUserBySession(token);
			if (id == -1)
			{
				*code = 2;
				*out = "{\"error\":\"Session invalid\"}";
				return;
			}
			User* user = getUserById(userId);
			if ((long)user == 0)
			{
				*code = 3;
				*out = "{\"error\":\"No user with that id\"}";
				return;
			}
			std::ostringstream oss;
			oss << "{" <<
				"\"userId\":\"" << std::to_string(id) << "\"," <<
				"\"name\":\"" << user->name << "\"," <<
				"\"color\":\"" << std::to_string(user->color) << "\"," <<
				"\"allianceId\":\"" << std::to_string(user->allianceId) << "\"," <<
				"\"cityLocation\":\"" << std::to_string(user->cityLocation) << "\"," <<
				"\"path\":\"" << std::to_string(user->path) << "\","
				"\"cityBuildingSlots\":" << "[" <<
				"\"" << std::to_string(user->cityBuildingSlots[0]) << "\"," <<
				"\"" << std::to_string(user->cityBuildingSlots[1]) << "\"," <<
				"\"" << std::to_string(user->cityBuildingSlots[2]) << "\"," <<
				"\"" << std::to_string(user->cityBuildingSlots[3]) << "\"," <<
				"\"" << std::to_string(user->cityBuildingSlots[4]) << "\"," <<
				"\"" << std::to_string(user->cityBuildingSlots[5]) << "\"," <<
				"\"" << std::to_string(user->cityBuildingSlots[6]) << "\"," <<
				"\"" << std::to_string(user->cityBuildingSlots[7]) << "\"" <<
				"]" <<
				"}";

			*out = oss.str();
			*code = 0;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
		void getSelfPlayer(long long token, std::string* out, int* code)
		{
			try{
			//std::cout << "getSelfPlayer m" << std::endl;
			int id = findUserBySession(token);
			if (id == -1)
			{
				*code = 2;
				*out = "No session found";
				return;
			}
			User* user = getUserById(id);
			std::string playerName = user->name;
			int color = user->color;
			int alliance = user->allianceId;
			int cityLocation = user->cityLocation;
			std::string userName = user->name;
			BYTE* citySlots = user->cityBuildingSlots;

			std::ostringstream oss;
			oss << "{" <<
				"\"userId\":\"" << std::to_string(id) << "\"," <<
				"\"name\":\"" << userName << "\"," <<
				"\"color\":\"" << std::to_string(color) << "\"," <<
				"\"allianceId\":\"" << std::to_string(alliance) << "\"," <<
				"\"path\":\"" << std::to_string(user->path) << "\"," <<
				"\"cityLocation\":\"" << std::to_string(cityLocation) << "\"," <<
				"\"cityUpgradeCap\":\"" << std::to_string(user->cityUpgradeCap) << "\"," <<
				"\"archerLevel\":\"" << std::to_string(user->archerLevel) << "\"," <<
				"\"swordLevel\":\"" << std::to_string(user->swordLevel) << "\"," <<
				"\"cavalryLevel\":\"" << std::to_string(user->cavalryLevel) << "\"," <<
				"\"spearmanLevel\":\"" << std::to_string(user->spearmanLevel) << "\"," <<
				"\"population\":\"" << std::to_string(user->population) << "\"," <<
				"\"populationCap\":\"" << std::to_string(user->populationCap) << "\"," <<
				"\"cityBuildingSlots\":[";
				for (int k = 0; k < 8; ++k)
				{
					if (k > 0) oss << ",";
					oss << "\"" << std::to_string(citySlots[k]) << "\"";
				}
				oss << "]," <<
				"\"buildingPositions\":[";
				for (int k = 0; k < user->mapBuildings.size(); ++k)
				{
					if (k > 0) oss << ",";
					int position = user->mapBuildings[k];
					oss << "{\"building\":\"" << std::to_string(Map::getTile(position)->building) << "\","
						<< "\"position\":\"" << position << "\"}";
				}
			oss << "]}";
			*out = oss.str();
			*code = 0;
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
	}
}