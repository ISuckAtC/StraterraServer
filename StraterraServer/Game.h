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
	namespace Player
	{
		class User;
	}
	namespace Game
	{
		static std::string indexDoc;
		static char favicon[32000];
		static size_t faviconLength;
		static bool running = true;
		int getTickInterval();
		int getTicksPerHour();

		class Session
		{
		public:
			Session(int playerId, long long token);
			int playerId;
			long long token;
			time_t lastSeen;

			std::string tokenBytes();
		};
		void removeSession(int userId);
		Straterra::Player::User* getUserAt(int index);
		int getUserCount();
		bool getUserOnline(int id);
		int getUserOnlineCount();
		int findUserBySession(long long token);
		Straterra::Player::User* getUserById(int id);
		Straterra::Player::User* getUserBySession(long long token);
		std::string getTokenString(long long token);
		long long getTokenLong(std::string tokenBytes);
		void addSession(Session* session);
		void addUser(Player::User* user);
		int createUserId();
		long long createSessionToken();
		void setLastStartIndex(int index);
		void addStartLocation(int position);
		int getNextStartLocation();

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

		bool Fight(std::vector<Group>* unitsLeft, std::vector<Group>* loserUnitsLeft, std::string* output, std::vector<Group> defender, std::vector<Group> attacker, bool city, bool verbose = false);
	}
}

#endif