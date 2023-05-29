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
		void logout(long long token, std::string* out, int* code);
		void stationArmy(long long token, int position, std::string units, std::string* out, int* code);
		void recallArmy(long long token, int position, std::string units, std::string* out, int* code);
		void upgradeStorage(long long token, int upgradeIndex, std::string* out, int* code);
		void removeNotification(long long token, int reportIndex, std::string* out, int* code);
		void removeAllNotifications(long long token, std::string* out, int* code);
		void viewNotification(long long token, int reportIndex, std::string* out, int* code);
		void getNotifications(long long token, std::string* out, int* code);
		void getUpdate(long long token, std::string* out, int* code);
		void setUserColor(long long token, int color, std::string* out, int* code);
		void upgradeUnit(long long token, int unitId, std::string* out, int* code);
		void choosePath(long long token, int choice, std::string* out, int* code);
		void attackMapTile(long long token, int position, std::string units, std::string* out, int* code);
		void getMapTile(long long token, int position, std::string* out, int* code);
		void getUser(long long token, int userId, std::string* out, int* code);
		void getResources(long long token, int userId, std::string* out, int* code);
		void getPlayers(long long token, std::string* out, int* code);
		void getSelfPlayer(long long token, std::string* out, int* code);
		void login(std::string* out, int* code, std::string loginInfo);
		//long long createSessionToken();
		void createUser(std::string name, std::string loginInfo, int cityLocation, std::string* out, int* code);
		void createTownBuilding(long long token, int buildingId, int buildingSlot, std::string* out, int* code);
		void createUnits(long long token, int unitId, int amount, std::string* out, int* code);
		void createMapBuilding(long long token, int buildingId, int position, std::string* out, int* code);
		void getScheduledEvents(long long token, std::string* out, int* code);
		void getHomeUnits(long long token, std::string* out, int* code);
	}
}

#endif