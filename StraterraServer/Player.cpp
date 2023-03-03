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

using namespace Straterra::Game;

namespace Straterra
{
	namespace Player
	{
		int createUserId()
		{
			int id;
			do
			{
				id = rand();
			} while (getUser(id)->userId != -1);
			return id;
		}
		long long createSessionToken()
		{
			long long token = 0;
			do
			{
				token = rand();
			} while (findUserBySession(token) != -1);
			return token;
		}
		void createUser(std::string name, std::string loginInfo)
		{
			std::cout << "[createUser] name: \"" << name << "\" login: \"" << loginInfo << "\"" << std::endl;
			User* u = new User();
			u->allianceId = 0;
			u->name = name;
			u->login = loginInfo;
			u->color = 0;
			u->cityLocation = 0;
			u->cityBuildingSlots[0] = 0;
			u->cityBuildingSlots[1] = 3;
			u->userId = createUserId();
			addUser(u);
		}
		void login(std::string* out, int* code, std::string loginInfo)
		{
			std::cout << getUserCount() << std::endl;
			for (int i = 0; i < getUserCount(); ++i)
			{
				User* u = getUserAt(i);
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

						*code = 0;
						*out = getTokenString(token);
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
			*out = "No player with that login";
		}
		void getSelfPlayer(long long token, std::string* out, int* code)
		{
			std::cout << "getSelfPlayer m" << std::endl;
			int id = findUserBySession(token);
			if (id == -1)
			{
				*code = 2;
				*out = "No session found";
				return;
			}
			Straterra::Game::User* user = getUser(id);
			std::string playerName = user->name;
			int color = user->color;
			int alliance = user->allianceId;
			int cityLocation = user->cityLocation;

			std::ostringstream oss;
			oss << "{" <<
				"\"userId\":\"" << playerName << "\"," <<
				"\"\":\"" << std::to_string(color) << "\"," <<
				"\"allianceId\":\"" << std::to_string(alliance) << "\"," <<
				"\"cityLocation\":\"" << std::to_string(cityLocation) << "\"}";
			*out = oss.str();
			*code = 0;
		}
	}
}