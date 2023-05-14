#ifndef DEFINITION_H
#define DEFINITION_H

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
	namespace Definition
	{

		enum UPGRADETYPE
		{
			STORAGEUPGRADE = 0,
			SMITHYUPGRADE = 1
		};
		struct InternalUpgrade
		{
			int id;
			UPGRADETYPE type;
			int upgradeTime;
			int prerequisite;
			int foodCost;
			int woodCost;
			int metalCost;
			int orderCost;
		};

		struct StorageUpgrade : InternalUpgrade
		{
			int foodMax;
			int woodMax;
			int metalMax;
			int orderMax;
		};

		InternalUpgrade* getInternalUpgradeDefinition(int id);

		void DefineInternalUpgrades();

		enum MAPBUILDINGTYPE
		{
			VILLAGE = 0,
			FARM = 1,
			WOOD = 2,
			MINE = 3,
			HOUSE = 4,
			CASTLE = 5
		};

		struct MapBuilding
		{
			int id;
			MAPBUILDINGTYPE type;
			int buildingTime;
			std::string name;
			int health;
			BYTE level;
			BYTE maxLevel;
			int foodCost;
			int woodCost;
			int metalCost;
			int orderCost;
			int baseProduction;
		};

		MapBuilding getMapBuildingDefinition(int id);

		void DefineMapBuildings();

		enum TOWNBUILDINGTYPE
		{
			TOWNHALL = 0,
			BARRACKS = 1,
			SMITHY = 2,
			ACADEMY = 3,
			TEMPLE = 4,
			WORKSHOP = 5,
			WAREHOUSE = 6,
			WALL = 7,
			MARKETPLACE = 8
		};

		struct TownBuilding
		{
			TownBuilding();
			int id;
			TOWNBUILDINGTYPE type;
			int buildingTime;
			std::string name;
			int health;
			BYTE level;
			BYTE maxLevel;
			int foodCost;
			int woodCost;
			int metalCost;
			int orderCost;
		};

		TownBuilding getTownBuildingDefinition(int id);

		void DefineTownBuildings();


		enum UNITTYPE
		{
			TYPELESS = 0,
			INFANTRY = 1,
			MISSILE = 2,
			CAVALRY = 3
		};
		struct Unit
		{
			int id;
			std::string name;
			int trainingTime;
			int unitType;
			int preference;
			BYTE level;
			BYTE maxLevel;
			BYTE speed;
			BYTE range;
			BYTE meleeAttack;
			BYTE rangeAttack;
			BYTE meleeDefence;
			BYTE rangeDefence;
			BYTE health;
			std::vector<int> bonusDamageTargets;
			std::vector<int> bonusDamageValues;
			int bonusDamageCount;
			BYTE counterBonus;
			int foodCost;
			int woodCost;
			int metalCost;
			int orderCost;
			int upgradeWoodCost;
			int upgradeFoodCost;
			int upgradeMetalCost;
			int upgradeOrderCost;
			int upgradeTime;

			int GetBonusDamage(int targetType);
		};

		Unit getUnitDefinition(int i);

		void DefineUnits();
	}
}
#endif