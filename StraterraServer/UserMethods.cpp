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

#include "Game.h"
#include "EventHub.h"
#include "Player.h"
#include "Definition.h"
#include "ScheduledEvent.h"

using namespace Straterra::Game;
using namespace Straterra::Player;
using namespace Straterra::Definition;
using namespace Straterra::ScheduledEvents;

namespace Straterra
{
	namespace UserMethods
	{
		void getScheduledEvents(long long token, std::string* out, int* code)
		{
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
					"\"running\":\"" << user->activeEvents[i]->running << "\"";
				switch (user->activeEvents[i]->type)
				{
				case UNITPRODUCTION:
				{
					ScheduledUnitProductionEvent* uProdEvent = (ScheduledUnitProductionEvent*)(user->activeEvents[i]);
					oss << "\"unitId\":\"" << uProdEvent->unitId << "\"," <<
						"\"amount\":\"" << uProdEvent->amount << "\"";
					break;
				}
				case TOWNBUILDING:
				{
					ScheduledTownBuildingEvent* bEvent = (ScheduledTownBuildingEvent*)(user->activeEvents[i]);
					oss << "\"buildingId\":\"" << bEvent->buildingId << "\"," <<
						"\"buildingSlot\":\"" << bEvent->buildingSlot << "\"";
					break;
				}
				}
				oss << "}";
			}
			oss << "]}";

			*out = oss.str();
			*code = 0;
		}
		void createBuilding(long long token, int buildingId, int buildingSlot, std::string* out, int* code)
		{
			User* user = getUserBySession(token);
			if (user->userId == -1)
			{
				*out = "{\"success\":\"false\",\"message\":\"Session invalid\"}";
				*code = 2;
				return;
			}

			TownBuilding townBuilding = getTownBuildingDefinition(buildingId);

			if (townBuilding.foodCost > user->food ||
				townBuilding.woodCost > user->wood ||
				townBuilding.metalCost > user->metal ||
				townBuilding.orderCost > user->order)
			{
				*out = "{\"success\":\"false\",\"message\":\"Not enough resources\"}";
				*code = 3;
				return;
			}

			user->food -= townBuilding.foodCost;
			user->wood -= townBuilding.woodCost;
			user->metal -= townBuilding.metalCost;
			user->order -= townBuilding.orderCost;

			ScheduledTownBuildingEvent buildEvent{ townBuilding.buildingTime, townBuilding.id, buildingSlot, user->userId };

			*out = "{\"success\":\"true\",\"message\":\"All good here!\"}";
			*code = 3;
		}

		void createUser(std::string name, std::string loginInfo)
		{
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
		void login(std::string* out, int* code, std::string loginInfo)
		{
			std::cout << "Users total: " << getUserCount() << std::endl;
			for (int i = 0; i < getUserCount(); ++i)
			{
				Player::User* u = getUserAt(i);
				std::cout << std::to_string((long)u) << " | ";
				std::cout << "name: \"" << u->name << "\" | login: \"" << u->login << "\"" << std::endl;
				try
				{
					std::cout << "user login: " << u->login << " | provided: " << loginInfo << std::endl;
					std::cout << "==: " << (u->login == loginInfo) << " | compare: " << (loginInfo.compare(u->login)) << std::endl;
					if (u->login == loginInfo)
					{
						long long token = createSessionToken();
						std::cout << token << std::endl;
						Session* s = new Session(u->userId, token);
						addSession(s);

						std::cout << "session token created: " << s->token << " (" << token << ")" << std::endl;
						std::cout << "token string: " << getTokenString(token) << std::endl;
						std::cout << "token string to long again: " << getTokenLong(getTokenString(token)) << std::endl;


						std::cout << "Users online (including you): " << getUserOnlineCount() << std::endl;
						*code = 0;
						*out = "{\"success\":\"true\",\"message\":\"" + getTokenString(token) + "\"}";
						return;
					}
					std::cout << "b" << std::endl;
				}
				catch (std::exception const& e)
				{
					std::cerr << "ERROR: " << e.what() << std::endl;
				}
			}

			*code = 2;
			*out = "{\"success\":\"false\",\"message\":\"No player with that login\"}";
		}
		void getResources(long long token, int userId, std::string* out, int* code)
		{
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
		void getPlayers(long long token, std::string* out, int* code)
		{
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
				oss << "{\"userId\":\"" << std::to_string(user->userId) << "\","
					<< "\"name\":\"" << user->name << "\","
					<< "\"cityLocation\":\"" << std::to_string(user->cityLocation) << "\"}";
				if (i < getUserCount() - 1) oss << ",";
			}
			oss << "]}";
			*out = oss.str();
			*code = 0;
		}
		void getUser(long long token, int userId, std::string* out, int* code)
		{
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
			}
			std::ostringstream oss;
			oss << "{" <<
				"\"userId\":\"" << std::to_string(id) << "\"," <<
				"\"name\":\"" << user->name << "\"," <<
				"\"color\":\"" << std::to_string(user->color) << "\"," <<
				"\"allianceId\":\"" << std::to_string(user->allianceId) << "\"," <<
				"\"cityLocation\":\"" << std::to_string(user->cityLocation) << "\"," <<
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
		void getSelfPlayer(long long token, std::string* out, int* code)
		{
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
				"\"" << std::to_string(citySlots[7]) << "\"" <<
				"]" <<
				"}";
			*out = oss.str();
			*code = 0;
		}
	}
}