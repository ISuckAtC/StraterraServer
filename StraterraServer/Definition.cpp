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

#include "Definition.h"

namespace Straterra
{
	namespace Definition
	{
		Unit unitDefinitions[256];
		TownBuilding townBuildingDefinitions[256];
        MapBuilding mapBuildingDefinitions[256];

		Unit getUnitDefinition(int i)
		{
			return unitDefinitions[i];
		}
		TownBuilding getTownBuildingDefinition(int id)
		{
			return townBuildingDefinitions[id];
		}
        MapBuilding getMapBuildingDefinition(int id)
        {
            return mapBuildingDefinitions[id];
        }

        void DefineMapBuildings()
        {
            MapBuilding village1;
            village1.type = VILLAGE;
            village1.buildingTime = 0;
            village1.foodCost = 0;
            village1.metalCost = 0;
            village1.woodCost = 0;
            village1.health = 15000;
            village1.id = 1;
            village1.level = 0;
            village1.maxLevel = 0;
            village1.name = "Village";
            village1.orderCost = 0;
            village1.baseProduction = 0;

            mapBuildingDefinitions[1] = village1;

            // Farms
            MapBuilding farm1;
            farm1.type = FARM;
            farm1.buildingTime = 100;
            farm1.foodCost = 0;
            farm1.metalCost = 0;
            farm1.woodCost = 200;
            farm1.health = 5000;
            farm1.id = 10;
            farm1.level = 1;
            farm1.maxLevel = 3;
            farm1.name = "Farm";
            farm1.orderCost = 0;
            farm1.baseProduction = 3600;

            mapBuildingDefinitions[10] = farm1;


            MapBuilding farm2;
            farm2.type = FARM;
            farm2.buildingTime = 700;
            farm2.foodCost = 0;
            farm2.metalCost = 0;
            farm2.woodCost = 700;
            farm2.health = 7000;
            farm2.id = 11;
            farm2.level = 2;
            farm2.maxLevel = 3;
            farm2.name = "Farm";
            farm2.orderCost = 0;
            farm2.baseProduction = 0;

            mapBuildingDefinitions[11] = farm2;


            MapBuilding farm3;
            farm3.type = FARM;
            farm3.buildingTime = 1100;
            farm3.foodCost = 0;
            farm3.metalCost = 600;
            farm3.woodCost = 1200;
            farm3.health = 9000;
            farm3.id = 12;
            farm3.level = 3;
            farm3.maxLevel = 3;
            farm3.name = "Farm";
            farm3.orderCost = 0;
            farm3.baseProduction = 0;

            mapBuildingDefinitions[12] = farm3;



            // Logging Camps
            MapBuilding wood1;
            wood1.type = WOOD;
            wood1.buildingTime = 200;
            wood1.foodCost = 300;
            wood1.metalCost = 0;
            wood1.woodCost = 0;
            wood1.health = 5000;
            wood1.id = 20;
            wood1.level = 1;
            wood1.maxLevel = 3;
            wood1.name = "Logging Camp";
            wood1.orderCost = 0;
            wood1.baseProduction = 3600;

            mapBuildingDefinitions[20] = wood1;


            MapBuilding wood2;
            wood2.type = WOOD;
            wood2.buildingTime = 500;
            wood2.foodCost = 0;
            wood2.metalCost = 200;
            wood2.woodCost = 800;
            wood2.health = 7000;
            wood2.id = 21;
            wood2.level = 2;
            wood2.maxLevel = 3;
            wood2.name = "Logging Camp";
            wood2.orderCost = 0;
            wood2.baseProduction = 0;

            mapBuildingDefinitions[21] = wood2;


            MapBuilding wood3;
            wood3.type = WOOD;
            wood3.buildingTime = 900;
            wood3.foodCost = 0;
            wood3.metalCost = 500;
            wood3.woodCost = 1300;
            wood3.health = 9000;
            wood3.id = 22;
            wood3.level = 3;
            wood3.maxLevel = 3;
            wood3.name = "Logging Camp";
            wood3.orderCost = 0;
            wood3.baseProduction = 0;

            mapBuildingDefinitions[22] = wood3;



            // Mines
            MapBuilding mine1;
            mine1.type = MINE;
            mine1.buildingTime = 250;
            mine1.foodCost = 0;
            mine1.metalCost = 100;
            mine1.woodCost = 300;
            mine1.health = 5000;
            mine1.id = 30;
            mine1.level = 1;
            mine1.maxLevel = 3;
            mine1.name = "Mine";
            mine1.orderCost = 0;
            mine1.baseProduction = 3600;

            mapBuildingDefinitions[30] = mine1;


            MapBuilding mine2;
            mine2.type = MINE;
            mine2.buildingTime = 250;
            mine2.foodCost = 0;
            mine2.metalCost = 300;
            mine2.woodCost = 700;
            mine2.health = 5000;
            mine2.id = 31;
            mine2.level = 2;
            mine2.maxLevel = 3;
            mine2.name = "Mine";
            mine2.orderCost = 0;
            mine2.baseProduction = 0;

            mapBuildingDefinitions[31] = mine2;


            MapBuilding mine3;
            mine3.type = MINE;
            mine3.buildingTime = 250;
            mine3.foodCost = 0;
            mine3.metalCost = 900;
            mine3.woodCost = 900;
            mine3.health = 5000;
            mine3.id = 32;
            mine3.level = 3;
            mine3.maxLevel = 3;
            mine3.name = "Mine";
            mine3.orderCost = 0;
            mine3.baseProduction = 0;

            mapBuildingDefinitions[32] = mine3;
        }

		void DefineTownBuildings()
		{
            TownBuilding townhall1;
            townhall1.id = 0;
            townhall1.type = TOWNHALL;
            townhall1.buildingTime = 5;
            townhall1.name = "Town Hall";
            townhall1.health = 1000;
            townhall1.woodCost = 50;
            townhall1.level = 1;
            townhall1.maxLevel = 2;
            townBuildingDefinitions[0] = townhall1;

            TownBuilding townhall2;
            townhall2.id = 1;
            townhall2.type = TOWNHALL;
            townhall2.buildingTime = 60;
            townhall2.name = "Town Hall";
            townhall2.health = 2000;
            townhall2.woodCost = 300;
            townhall2.level = 2;
            townhall2.maxLevel = 3;
            townBuildingDefinitions[1] = townhall2;

            TownBuilding townhall3;
            townhall3.id = 2;
            townhall3.type = TOWNHALL;
            townhall3.buildingTime = 240;
            townhall3.name = "Town Hall";
            townhall3.health = 3000;
            townhall3.woodCost = 600;
            townhall3.level = 3;
            townhall3.maxLevel = 3;
            townBuildingDefinitions[1] = townhall3;

            TownBuilding barracks1;
            barracks1.id = 3;
            barracks1.type = BARRACKS;
            barracks1.buildingTime = 30;
            barracks1.name = "Barracks";
            barracks1.health = 500;
            barracks1.woodCost = 50;
            barracks1.metalCost = 50;
            barracks1.level = 1;
            barracks1.maxLevel = 2;
            townBuildingDefinitions[3] = barracks1;

            TownBuilding barracks2;
            barracks2.id = 4;
            barracks2.type = BARRACKS;
            barracks2.buildingTime = 100;
            barracks2.name = "Barracks";
            barracks2.health = 1500;
            barracks2.woodCost = 300;
            barracks2.metalCost = 300;
            barracks2.level = 2;
            barracks2.maxLevel = 3;
            townBuildingDefinitions[4] = barracks2;

            TownBuilding barracks3;
            barracks3.id = 5;
            barracks3.type = BARRACKS;
            barracks3.buildingTime = 300;
            barracks3.name = "Barracks";
            barracks3.health = 2500;
            barracks3.woodCost = 600;
            barracks3.metalCost = 600;
            barracks3.level = 3;
            barracks3.maxLevel = 3;
            townBuildingDefinitions[5] = barracks3;

            TownBuilding smithy1;
            smithy1.id = 6;
            smithy1.type = SMITHY;
            smithy1.buildingTime = 5;
            smithy1.name = "Smithy";
            smithy1.health = 1000;
            smithy1.woodCost = 200;
            smithy1.level = 1;
            smithy1.maxLevel = 2;
            townBuildingDefinitions[6] = smithy1;

            TownBuilding smithy2;
            smithy2.id = 7;
            smithy2.type = SMITHY;
            smithy2.buildingTime = 60;
            smithy2.name = "Smithy";
            smithy2.health = 2000;
            smithy2.woodCost = 600;
            smithy2.level = 2;
            smithy2.maxLevel = 3;
            townBuildingDefinitions[7] = smithy2;

            TownBuilding smithy3;
            smithy3.id = 8;
            smithy3.type = SMITHY;
            smithy3.buildingTime = 240;
            smithy3.name = "Smithy";
            smithy3.health = 3000;
            smithy3.woodCost = 1200;
            smithy3.level = 3;
            smithy3.maxLevel = 3;
            townBuildingDefinitions[8] = smithy3;

            TownBuilding academy1;
            academy1.id = 9;
            academy1.type = ACADEMY;
            academy1.buildingTime = 5;
            academy1.name = "Academy";
            academy1.health = 750;
            academy1.woodCost = 200;
            academy1.metalCost = 100;
            academy1.level = 1;
            academy1.maxLevel = 2;
            townBuildingDefinitions[9] = academy1;

            TownBuilding academy2;
            academy2.id = 10;
            academy2.type = ACADEMY;
            academy2.buildingTime = 60;
            academy2.name = "Academy";
            academy2.health = 1750;
            academy2.woodCost = 400;
            academy2.metalCost = 200;
            academy2.level = 2;
            academy2.maxLevel = 3;
            townBuildingDefinitions[10] = academy2;

            TownBuilding academy3;
            academy3.id = 11;
            academy3.type = ACADEMY;
            academy3.buildingTime = 240;
            academy3.name = "Academy";
            academy3.health = 2750;
            academy3.woodCost = 800;
            academy3.metalCost = 500;
            academy3.level = 3;
            academy3.maxLevel = 3;
            townBuildingDefinitions[11] = academy3;

            TownBuilding temple1;
            temple1.id = 12;
            temple1.type = TEMPLE;
            temple1.buildingTime = 5;
            temple1.name = "Temple";
            temple1.health = 400;
            temple1.woodCost = 200;
            temple1.level = 1;
            temple1.maxLevel = 2;
            townBuildingDefinitions[12] = temple1;

            TownBuilding temple2;
            temple2.id = 13;
            temple2.type = TEMPLE;
            temple2.buildingTime = 60;
            temple2.name = "Temple";
            temple2.health = 800;
            temple2.woodCost = 500;
            temple2.level = 2;
            temple2.maxLevel = 3;
            townBuildingDefinitions[13] = temple2;

            TownBuilding temple3;
            temple3.id = 14;
            temple3.type = TEMPLE;
            temple3.buildingTime = 190;
            temple3.name = "Temple";
            temple3.health = 1200;
            temple3.woodCost = 1000;
            temple3.level = 3;
            temple3.maxLevel = 3;
            townBuildingDefinitions[14] = temple3;

            TownBuilding workshop1;
            workshop1.id = 15;
            workshop1.type = WORKSHOP;
            workshop1.buildingTime = 5;
            workshop1.name = "Workshop";
            workshop1.health = 1100;
            workshop1.woodCost = 200;
            workshop1.level = 1;
            workshop1.maxLevel = 2;
            townBuildingDefinitions[15] = workshop1;

            TownBuilding workshop2;
            workshop2.id = 16;
            workshop2.type = WORKSHOP;
            workshop2.buildingTime = 50;
            workshop2.name = "Workshop";
            workshop2.health = 2200;
            workshop2.woodCost = 500;
            workshop2.level = 2;
            workshop2.maxLevel = 3;
            townBuildingDefinitions[16] = workshop2;

            TownBuilding workshop3;
            workshop3.id = 17;
            workshop3.type = WORKSHOP;
            workshop3.buildingTime = 250;
            workshop3.name = "Workshop";
            workshop3.health = 3300;
            workshop3.woodCost = 1000;
            workshop3.level = 3;
            workshop3.maxLevel = 3;
            townBuildingDefinitions[17] = workshop3;

            TownBuilding warehouse1;
            warehouse1.id = 18;
            warehouse1.type = WAREHOUSE;
            warehouse1.buildingTime = 5;
            warehouse1.name = "Warehouse";
            warehouse1.health = 1600;
            warehouse1.woodCost = 10;
            warehouse1.level = 1;
            warehouse1.maxLevel = 2;
            townBuildingDefinitions[18] = warehouse1;

            TownBuilding warehouse2;
            warehouse2.id = 19;
            warehouse2.type = WAREHOUSE;
            warehouse2.buildingTime = 55;
            warehouse2.name = "Warehouse";
            warehouse2.health = 2400;
            warehouse2.woodCost = 800;
            warehouse2.level = 2;
            warehouse2.maxLevel = 3;
            townBuildingDefinitions[19] = warehouse2;

            TownBuilding warehouse3;
            warehouse3.id = 20;
            warehouse3.type = WAREHOUSE;
            warehouse3.buildingTime = 185;
            warehouse3.name = "Warehouse";
            warehouse3.health = 3600;
            warehouse3.woodCost = 2000;
            warehouse3.level = 3;
            warehouse3.maxLevel = 3;
            townBuildingDefinitions[20] = warehouse3;

            TownBuilding marketplace1;
            marketplace1.id = 24;
            marketplace1.type = MARKETPLACE;
            marketplace1.buildingTime = 5;
            marketplace1.name = "Marketplace";
            marketplace1.health = 500;
            marketplace1.foodCost = 200;
            marketplace1.woodCost = 200;
            marketplace1.metalCost = 200;
            marketplace1.level = 1;
            marketplace1.maxLevel = 2;
            townBuildingDefinitions[24] = marketplace1;

            TownBuilding marketplace2;
            marketplace2.id = 25;
            marketplace2.type = MARKETPLACE;
            marketplace2.buildingTime = 70;
            marketplace2.name = "Marketplace";
            marketplace2.health = 1000;
            marketplace2.foodCost = 400;
            marketplace2.woodCost = 400;
            marketplace2.metalCost = 400;
            marketplace2.level = 2;
            marketplace2.maxLevel = 3;
            townBuildingDefinitions[25] = marketplace2;

            TownBuilding marketplace3;
            marketplace3.id = 26;
            marketplace3.type = MARKETPLACE;
            marketplace3.buildingTime = 130;
            marketplace3.name = "Marketplace";
            marketplace3.health = 1500;
            marketplace1.foodCost = 1000;
            marketplace1.woodCost = 1000;
            marketplace1.metalCost = 1000;
            marketplace3.level = 3;
            marketplace3.maxLevel = 3;
            townBuildingDefinitions[26] = marketplace3;
		}

		void DefineUnits()
		{
			Unit archer;
			archer.id = 0;
			archer.trainingTime = 1;
			archer.preference = INFANTRY;
			archer.unitType = MISSILE;
			archer.level = 1;
			archer.maxLevel = 1;
			archer.speed = 2;
			archer.range = 10;
			archer.meleeAttack = 2;
			archer.rangeAttack = 7;
			archer.meleeDefence = 2;
			archer.rangeDefence = 1;
			archer.health = 10;
			archer.bonusDamageCount = 0;
			archer.counterBonus = 0;
			archer.foodCost = 40;
			archer.woodCost = 50;
			archer.metalCost = 5;
			unitDefinitions[0] = archer;

			Unit cavalry;
			cavalry.id = 1;
			cavalry.trainingTime = 60;
			cavalry.preference = MISSILE;
			cavalry.unitType = CAVALRY;
			cavalry.level = 1;
			cavalry.maxLevel = 1;
			cavalry.speed = 6;
			cavalry.range = 0;
			cavalry.meleeAttack = 14;
			cavalry.rangeAttack = 0;
			cavalry.meleeDefence = 4;
			cavalry.rangeDefence = 3;
			cavalry.health = 50;
			cavalry.bonusDamageCount = 0;
			cavalry.counterBonus = 0;
			cavalry.foodCost = 200;
			cavalry.metalCost = 50;
			unitDefinitions[1] = cavalry;

			// cavalry.bonusDamage.Add(UnitType.INFANTRY, 2);

			Unit swordsman;
			swordsman.id = 2;
			swordsman.trainingTime = 5;
			swordsman.preference = INFANTRY;
			swordsman.unitType = INFANTRY;
			swordsman.level = 1;
			swordsman.maxLevel = 1;
			swordsman.speed = 2;        //4 should be standard footspeed for footsoldiers.
			swordsman.range = 0;
			swordsman.meleeAttack = 8;
			swordsman.rangeAttack = 0;
			swordsman.meleeDefence = 5;
			swordsman.rangeDefence = 4;
			swordsman.health = 15;
			swordsman.bonusDamageCount = 0;
			swordsman.counterBonus = 0;
			swordsman.foodCost = 50;
			swordsman.metalCost = 10;
			unitDefinitions[2] = swordsman;

			Unit spearman;
			spearman.id = 3;
			spearman.trainingTime = 3;
			spearman.preference = CAVALRY;
			spearman.unitType = INFANTRY;
			spearman.level = 1;
			spearman.maxLevel = 1;
			spearman.speed = 3;
			spearman.range = 0;
			spearman.meleeAttack = 7;
			spearman.rangeAttack = 0;
			spearman.meleeDefence = 3;
			spearman.rangeDefence = 2;       //Should probably have less.
			spearman.health = 12;
			spearman.bonusDamageCount = 1;
			spearman.counterBonus = 0;
			spearman.foodCost = 30;
			spearman.woodCost = 20;

			spearman.bonusDamageTargets = { CAVALRY };
			spearman.bonusDamageValues = { 10 };

			unitDefinitions[3] = spearman;
		}

		int Unit::GetBonusDamage(int targetType)
		{
			for (int i = 0; i < bonusDamageCount; ++i)
			{
				if (targetType == bonusDamageTargets[i])
				{
					return bonusDamageValues[i];
				}
			}
			return 0;
		}
	}
}