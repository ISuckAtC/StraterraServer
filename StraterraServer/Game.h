#ifndef GAME_H
#define GAME_H

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
			~User();

		private:

		};

		class Session
		{
		public:
			Session(int playerId, long long token);
			int playerId;
			long long token;
			time_t lastSeen;

			std::string tokenBytes();
		};

		User* getUserAt(int index);
		int getUserCount();
		int getUserOnlineCount();
		int findUserBySession(long long token);
		User* getUser(int id);
		std::string getTokenString(long long token);
		long long getTokenLong(std::string tokenBytes);
		void addSession(Session* session);
		void addUser(User* user);

		void start(int _tickInterval, int _timeOutSeconds);
		
		

		

		class Group
		{
		public:
			int count;
			int unitId;
			int frontHealth;
			int position;
			int target;
			bool right;
			bool dead;
			Group(int _count, int _unitId);

			bool operator==(Group other);

			int GetDamage(int range, Group* enemy, bool counter = false, bool trampleCounter = false);

			int TakeDamage(int damage, Group* source, bool melee, bool counterable = true, bool verbose = true, bool trample = false);
		private:

		};

		struct Tile
		{
			int location;
			int travelCost;
			BYTE tileType;
			BYTE building;
			int owner;
			float foodAmount;
			float woodAmount;
			float metalAmount;
			float chaosAmount;
			std::vector<Group> army;
		};

		bool Fight(std::vector<Group>* unitsLeft, std::string* output, std::vector<Group*> defender, std::vector<Group*> attacker, bool verbose = false);
	}
}
#endif