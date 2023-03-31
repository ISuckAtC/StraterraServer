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
		void choosePath(long long token, int choice, std::string* out, int* code)
		{
			try
			{
				// Grab and verify user
				User* user = getUserBySession(token);
				if (user->userId == -1)
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
				if (user->userId == -1)
				{
					*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
					*code = 2;
					return;
				}

				if (Map::getTile(destination)->building == 0)
				{
					*out = "{\"success\":\"false\",\"message\":\"Nothing to attack\"}";
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
			if (user->userId == -1)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			Map::Tile* tile = Map::getTile(position);
			
			std::ostringstream oss;
			oss << "{\"building\":\"" << tile->building << "\"}";

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
			if (user->userId == -1)
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
			if (user->userId == -1)
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
			if (user->userId == -1)
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
			if (user->userId == -1)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			// Grab the definition for the desired unit
			Unit unit = getUnitDefinition(unitId);

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

			// Use new to prevent the object from being destructed
			// We destroy it manually when the event completes
			new ScheduledUnitProductionEvent{ unit.trainingTime * amount, unit.id, amount, user->userId };

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
			if (user->userId == -1)
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
						*out = "{\"success\":\"false\",\"message\":\"Can't build here\"}";
						*code = 4;
						return;
					}
				}
				else
				{
					*out = "{\"success\":\"false\",\"message\":\"Can't build here\"}";
					*code = 4;
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

		void createUser(std::string name, std::string loginInfo)
		{
			try{
			std::cout << "[createUser] name: \"" << name << "\" login: \"" << loginInfo << "\"" << std::endl;
			Player::User* u = new Player::User();
			u->allianceId = 0;
			u->name = name;
			u->login = loginInfo;
			u->color = 0;
			u->cityLocation = 0;
			u->cityBuildingSlots[0] = 0;
			u->cityBuildingSlots[1] = 3;
			u->userId = Game::createUserId();

			u->food = 1000;
			u->wood = 1000;
			u->metal = 500;
			u->order = 50;

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
				//std::cout << "name: \"" << u->name << "\" | login: \"" << u->login << "\"" << std::endl;
				try
				{
					//std::cout << "user login: " << u->login << " | provided: " << loginInfo << std::endl;
					//std::cout << "==: " << (u->login == loginInfo) << " | compare: " << (loginInfo.compare(u->login)) << std::endl;
					if (u->login == loginInfo)
					{
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
			if (user->userId == -1)
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
			if (user->userId == -1)
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
				"\"cityLocation\":\"" << std::to_string(cityLocation) << "\"," <<
				"\"cityBuildingSlots\":" << "[" <<
				"\"" << std::to_string(citySlots[0]) << "\"," <<
				"\"" << std::to_string(citySlots[1]) << "\"," <<
				"\"" << std::to_string(citySlots[2]) << "\"," <<
				"\"" << std::to_string(citySlots[3]) << "\"," <<
				"\"" << std::to_string(citySlots[4]) << "\"," <<
				"\"" << std::to_string(citySlots[5]) << "\"," <<
				"\"" << std::to_string(citySlots[6]) << "\"," <<
				"\"" << std::to_string(citySlots[7]) << "\"]," <<
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