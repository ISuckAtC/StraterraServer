#pragma once
#ifndef USERMETHODS_H
#define USERMETHODS_H

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
	namespace UserMethods
	{
		void getUser(long long token, int userId, std::string* out, int* code);
		void getResources(long long token, int userId, std::string* out, int* code);
		void getPlayers(long long token, std::string* out, int* code);
		void getSelfPlayer(long long token, std::string* out, int* code);
		void login(std::string* out, int* code, std::string loginInfo);
		//long long createSessionToken();
		void createUser(std::string name, std::string loginInfo);
		void createTownBuilding(long long token, int buildingId, int buildingSlot, std::string* out, int* code);
		void createUnits(long long token, int unitId, int amount, std::string* out, int* code);
		void createMapBuilding(long long token, int buildingId, int position, std::string* out, int* code);
		void getScheduledEvents(long long token, std::string* out, int* code);
		void getHomeUnits(long long token, std::string* out, int* code);
	}
}

#endif