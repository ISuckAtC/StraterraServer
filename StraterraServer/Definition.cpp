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
            MapBuilding village1{};
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
            MapBuilding farm1{};
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


            MapBuilding farm2{};
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


            MapBuilding farm3{};
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
            MapBuilding wood1{};
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


            MapBuilding wood2{};
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


            MapBuilding wood3{};
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
            MapBuilding mine1{};
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


            MapBuilding mine2{};
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


            MapBuilding mine3{};
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
            TownBuilding townhall1{};
            townhall1.id = 0;
            townhall1.type = TOWNHALL;
            townhall1.buildingTime = 5;
            townhall1.name = "Town Hall";
            townhall1.health = 1000;
            townhall1.woodCost = 50;
            townhall1.level = 1;
            townhall1.maxLevel = 2;
            townBuildingDefinitions[0] = townhall1;

            TownBuilding townhall2{};
            townhall2.id = 1;
            townhall2.type = TOWNHALL;
            townhall2.buildingTime = 60;
            townhall2.name = "Town Hall";
            townhall2.health = 2000;
            townhall2.woodCost = 300;
            townhall2.level = 2;
            townhall2.maxLevel = 3;
            townBuildingDefinitions[1] = townhall2;

            TownBuilding townhall3{};
            townhall3.id = 2;
            townhall3.type = TOWNHALL;
            townhall3.buildingTime = 240;
            townhall3.name = "Town Hall";
            townhall3.health = 3000;
            townhall3.woodCost = 600;
            townhall3.level = 3;
            townhall3.maxLevel = 3;
            townBuildingDefinitions[1] = townhall3;

            TownBuilding barracks1{};
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

            TownBuilding barracks2{};
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

            TownBuilding barracks3{};
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

            TownBuilding smithy1{};
            smithy1.id = 6;
            smithy1.type = SMITHY;
            smithy1.buildingTime = 5;
            smithy1.name = "Smithy";
            smithy1.health = 1000;
            smithy1.woodCost = 200;
            smithy1.level = 1;
            smithy1.maxLevel = 2;
            townBuildingDefinitions[6] = smithy1;

            TownBuilding smithy2{};
            smithy2.id = 7;
            smithy2.type = SMITHY;
            smithy2.buildingTime = 60;
            smithy2.name = "Smithy";
            smithy2.health = 2000;
            smithy2.woodCost = 600;
            smithy2.level = 2;
            smithy2.maxLevel = 3;
            townBuildingDefinitions[7] = smithy2;

            TownBuilding smithy3{};
            smithy3.id = 8;
            smithy3.type = SMITHY;
            smithy3.buildingTime = 240;
            smithy3.name = "Smithy";
            smithy3.health = 3000;
            smithy3.woodCost = 1200;
            smithy3.level = 3;
            smithy3.maxLevel = 3;
            townBuildingDefinitions[8] = smithy3;

            TownBuilding academy1{};
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

            TownBuilding academy2{};
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

            TownBuilding academy3{};
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

            TownBuilding temple1{};
            temple1.id = 12;
            temple1.type = TEMPLE;
            temple1.buildingTime = 5;
            temple1.name = "Temple";
            temple1.health = 400;
            temple1.woodCost = 200;
            temple1.level = 1;
            temple1.maxLevel = 2;
            townBuildingDefinitions[12] = temple1;

            TownBuilding temple2{};
            temple2.id = 13;
            temple2.type = TEMPLE;
            temple2.buildingTime = 60;
            temple2.name = "Temple";
            temple2.health = 800;
            temple2.woodCost = 500;
            temple2.level = 2;
            temple2.maxLevel = 3;
            townBuildingDefinitions[13] = temple2;

            TownBuilding temple3{};
            temple3.id = 14;
            temple3.type = TEMPLE;
            temple3.buildingTime = 190;
            temple3.name = "Temple";
            temple3.health = 1200;
            temple3.woodCost = 1000;
            temple3.level = 3;
            temple3.maxLevel = 3;
            townBuildingDefinitions[14] = temple3;

            TownBuilding workshop1{};
            workshop1.id = 15;
            workshop1.type = WORKSHOP;
            workshop1.buildingTime = 5;
            workshop1.name = "Workshop";
            workshop1.health = 1100;
            workshop1.woodCost = 200;
            workshop1.level = 1;
            workshop1.maxLevel = 2;
            townBuildingDefinitions[15] = workshop1;

            TownBuilding workshop2{};
            workshop2.id = 16;
            workshop2.type = WORKSHOP;
            workshop2.buildingTime = 50;
            workshop2.name = "Workshop";
            workshop2.health = 2200;
            workshop2.woodCost = 500;
            workshop2.level = 2;
            workshop2.maxLevel = 3;
            townBuildingDefinitions[16] = workshop2;

            TownBuilding workshop3{};
            workshop3.id = 17;
            workshop3.type = WORKSHOP;
            workshop3.buildingTime = 250;
            workshop3.name = "Workshop";
            workshop3.health = 3300;
            workshop3.woodCost = 1000;
            workshop3.level = 3;
            workshop3.maxLevel = 3;
            townBuildingDefinitions[17] = workshop3;

            TownBuilding warehouse1{};
            warehouse1.id = 18;
            warehouse1.type = WAREHOUSE;
            warehouse1.buildingTime = 5;
            warehouse1.name = "Warehouse";
            warehouse1.health = 1600;
            warehouse1.woodCost = 10;
            warehouse1.level = 1;
            warehouse1.maxLevel = 2;
            townBuildingDefinitions[18] = warehouse1;

            TownBuilding warehouse2{};
            warehouse2.id = 19;
            warehouse2.type = WAREHOUSE;
            warehouse2.buildingTime = 55;
            warehouse2.name = "Warehouse";
            warehouse2.health = 2400;
            warehouse2.woodCost = 800;
            warehouse2.level = 2;
            warehouse2.maxLevel = 3;
            townBuildingDefinitions[19] = warehouse2;

            TownBuilding warehouse3{};
            warehouse3.id = 20;
            warehouse3.type = WAREHOUSE;
            warehouse3.buildingTime = 185;
            warehouse3.name = "Warehouse";
            warehouse3.health = 3600;
            warehouse3.woodCost = 2000;
            warehouse3.level = 3;
            warehouse3.maxLevel = 3;
            townBuildingDefinitions[20] = warehouse3;

            TownBuilding marketplace1{};
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

            TownBuilding marketplace2{};
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

            TownBuilding marketplace3{};
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
            Unit archer0{};
            archer0.id = 0;
            archer0.trainingTime = 1;
            archer0.name = "Archer";
            archer0.preference = INFANTRY;
            archer0.unitType = MISSILE;
            archer0.level = 1;
            archer0.speed = 2;
            archer0.range = 10;
            archer0.meleeAttack = 2;
            archer0.rangeAttack = 7;
            archer0.meleeDefence = 2;
            archer0.rangeDefence = 1;
            archer0.health = 10;
            archer0.bonusDamageCount = 0;
            archer0.counterBonus = 0;
            archer0.foodCost = 40;
            archer0.woodCost = 50;
            archer0.metalCost = 5;
            archer0.upgradeFoodCost = 0;
            archer0.upgradeWoodCost = 0;
            archer0.upgradeMetalCost = 0;
            archer0.upgradeOrderCost = 0;
            archer0.upgradeTime = 0;

            unitDefinitions[0] = archer0;

            Unit archer1{};
            archer1.id = 1;
            archer1.trainingTime = 1;
            archer1.name = "Archer";
            archer1.preference = INFANTRY;
            archer1.unitType = MISSILE;
            archer1.level = 2;
            archer1.speed = 2;
            archer1.range = 10;
            archer1.meleeAttack = 2;
            archer1.rangeAttack = 7;
            archer1.meleeDefence = 2;
            archer1.rangeDefence = 1;
            archer1.health = 15;
            archer1.bonusDamageCount = 0;
            archer1.counterBonus = 0;
            archer1.foodCost = 40;
            archer1.woodCost = 50;
            archer1.metalCost = 5;
            archer1.upgradeFoodCost = 100;
            archer1.upgradeWoodCost = 100;
            archer1.upgradeMetalCost = 100;
            archer1.upgradeOrderCost = 0;
            archer1.upgradeTime = 60;
            unitDefinitions[1] = archer1;

            Unit archer2{};
            archer2.id = 2;
            archer2.trainingTime = 1;
            archer2.name = "Archer";
            archer2.preference = INFANTRY;
            archer2.unitType = MISSILE;
            archer2.level = 3;
            archer2.speed = 2;
            archer2.range = 10;
            archer2.meleeAttack = 2;
            archer2.rangeAttack = 7;
            archer2.meleeDefence = 2;
            archer2.rangeDefence = 1;
            archer2.health = 20;
            archer2.bonusDamageCount = 0;
            archer2.counterBonus = 0;
            archer2.foodCost = 40;
            archer2.woodCost = 50;
            archer2.metalCost = 5;
            archer2.upgradeFoodCost = 100;
            archer2.upgradeWoodCost = 100;
            archer2.upgradeMetalCost = 100;
            archer2.upgradeOrderCost = 0;
            archer2.upgradeTime = 60;
            unitDefinitions[2] = archer2;

            Unit archer3{};
            archer3.id = 3;
            archer3.trainingTime = 1;
            archer3.name = "Archer";
            archer3.preference = INFANTRY;
            archer3.unitType = MISSILE;
            archer3.level = 4;
            archer3.speed = 2;
            archer3.range = 10;
            archer3.meleeAttack = 2;
            archer3.rangeAttack = 7;
            archer3.meleeDefence = 2;
            archer3.rangeDefence = 1;
            archer3.health = 30;
            archer3.bonusDamageCount = 0;
            archer3.counterBonus = 0;
            archer3.foodCost = 40;
            archer3.woodCost = 50;
            archer3.metalCost = 5;
            archer3.upgradeFoodCost = 100;
            archer3.upgradeWoodCost = 100;
            archer3.upgradeMetalCost = 100;
            archer3.upgradeOrderCost = 0;
            archer3.upgradeTime = 60;
            unitDefinitions[3] = archer3;

            Unit archer4{};
            archer4.id = 4;
            archer4.trainingTime = 1;
            archer4.name = "Archer";
            archer4.preference = INFANTRY;
            archer4.unitType = MISSILE;
            archer4.level = 5;
            archer4.speed = 2;
            archer4.range = 10;
            archer4.meleeAttack = 2;
            archer4.rangeAttack = 7;
            archer4.meleeDefence = 2;
            archer4.rangeDefence = 1;
            archer4.health = 10;
            archer4.bonusDamageCount = 0;
            archer4.counterBonus = 0;
            archer4.foodCost = 40;
            archer4.woodCost = 50;
            archer4.metalCost = 5;
            archer4.upgradeFoodCost = 100;
            archer4.upgradeWoodCost = 100;
            archer4.upgradeMetalCost = 100;
            archer4.upgradeOrderCost = 0;
            archer4.upgradeTime = 60;
            unitDefinitions[4] = archer4;

            Unit archer5{};
            archer5.id = 5;
            archer5.trainingTime = 1;
            archer5.name = "Archer";
            archer5.preference = INFANTRY;
            archer5.unitType = MISSILE;
            archer5.level = 6;
            archer5.speed = 2;
            archer5.range = 10;
            archer5.meleeAttack = 2;
            archer5.rangeAttack = 7;
            archer5.meleeDefence = 2;
            archer5.rangeDefence = 1;
            archer5.health = 10;
            archer5.bonusDamageCount = 0;
            archer5.counterBonus = 0;
            archer5.foodCost = 40;
            archer5.woodCost = 50;
            archer5.metalCost = 5;
            archer5.upgradeFoodCost = 100;
            archer5.upgradeWoodCost = 100;
            archer5.upgradeMetalCost = 100;
            archer5.upgradeOrderCost = 0;
            archer5.upgradeTime = 60;
            unitDefinitions[5] = archer5;


            Unit cavalry0{};
            cavalry0.id = 10;
            cavalry0.trainingTime = 60;
            cavalry0.name = "Cavalry";
            cavalry0.preference = MISSILE;
            cavalry0.unitType = CAVALRY;
            cavalry0.level = 1;
            cavalry0.speed = 6;
            cavalry0.range = 0;
            cavalry0.meleeAttack = 14;
            cavalry0.rangeAttack = 0;
            cavalry0.meleeDefence = 4;
            cavalry0.rangeDefence = 3;
            cavalry0.health = 50;
            cavalry0.bonusDamageCount = 0;
            cavalry0.counterBonus = 0;
            cavalry0.foodCost = 200;
            cavalry0.metalCost = 50;
            cavalry0.upgradeFoodCost = 100;
            cavalry0.upgradeWoodCost = 100;
            cavalry0.upgradeMetalCost = 100;
            cavalry0.upgradeOrderCost = 0;
            cavalry0.upgradeTime = 60;
            unitDefinitions[10] = cavalry0;

            Unit cavalry1{};
            cavalry1.id = 11;
            cavalry1.trainingTime = 60;
            cavalry1.name = "Cavalry";
            cavalry1.preference = MISSILE;
            cavalry1.unitType = CAVALRY;
            cavalry1.level = 2;
            cavalry1.speed = 6;
            cavalry1.range = 0;
            cavalry1.meleeAttack = 14;
            cavalry1.rangeAttack = 0;
            cavalry1.meleeDefence = 4;
            cavalry1.rangeDefence = 3;
            cavalry1.health = 60;
            cavalry1.bonusDamageCount = 0;
            cavalry1.counterBonus = 0;
            cavalry1.foodCost = 200;
            cavalry1.metalCost = 50;
            cavalry1.upgradeFoodCost = 100;
            cavalry1.upgradeWoodCost = 100;
            cavalry1.upgradeMetalCost = 100;
            cavalry1.upgradeOrderCost = 0;
            cavalry1.upgradeTime = 60;
            unitDefinitions[11] = cavalry1;

            Unit cavalry2{};
            cavalry2.id = 12;
            cavalry2.trainingTime = 60;
            cavalry2.name = "Cavalry";
            cavalry2.preference = MISSILE;
            cavalry2.unitType = CAVALRY;
            cavalry2.level = 3;
            cavalry2.speed = 6;
            cavalry2.range = 0;
            cavalry2.meleeAttack = 14;
            cavalry2.rangeAttack = 0;
            cavalry2.meleeDefence = 4;
            cavalry2.rangeDefence = 3;
            cavalry2.health = 50;
            cavalry2.bonusDamageCount = 0;
            cavalry2.counterBonus = 0;
            cavalry2.foodCost = 200;
            cavalry2.metalCost = 50;
            cavalry2.upgradeFoodCost = 100;
            cavalry2.upgradeWoodCost = 100;
            cavalry2.upgradeMetalCost = 100;
            cavalry2.upgradeOrderCost = 0;
            cavalry2.upgradeTime = 60;
            unitDefinitions[12] = cavalry2;

            Unit cavalry3{};
            cavalry3.id = 13;
            cavalry3.trainingTime = 60;
            cavalry3.name = "Cavalry";
            cavalry3.preference = MISSILE;
            cavalry3.unitType = CAVALRY;
            cavalry3.level = 4;
            cavalry3.speed = 6;
            cavalry3.range = 0;
            cavalry3.meleeAttack = 14;
            cavalry3.rangeAttack = 0;
            cavalry3.meleeDefence = 4;
            cavalry3.rangeDefence = 3;
            cavalry3.health = 50;
            cavalry3.bonusDamageCount = 0;
            cavalry3.counterBonus = 0;
            cavalry3.foodCost = 200;
            cavalry3.metalCost = 50;
            cavalry3.upgradeFoodCost = 100;
            cavalry3.upgradeWoodCost = 100;
            cavalry3.upgradeMetalCost = 100;
            cavalry3.upgradeOrderCost = 0;
            cavalry3.upgradeTime = 60;
            unitDefinitions[13] = cavalry3;

            Unit cavalry4{};
            cavalry4.id = 14;
            cavalry4.trainingTime = 60;
            cavalry4.name = "Cavalry";
            cavalry4.preference = MISSILE;
            cavalry4.unitType = CAVALRY;
            cavalry4.level = 5;
            cavalry4.speed = 6;
            cavalry4.range = 0;
            cavalry4.meleeAttack = 14;
            cavalry4.rangeAttack = 0;
            cavalry4.meleeDefence = 4;
            cavalry4.rangeDefence = 3;
            cavalry4.health = 50;
            cavalry4.bonusDamageCount = 0;
            cavalry4.counterBonus = 0;
            cavalry4.foodCost = 200;
            cavalry4.metalCost = 50;
            cavalry4.upgradeFoodCost = 100;
            cavalry4.upgradeWoodCost = 100;
            cavalry4.upgradeMetalCost = 100;
            cavalry4.upgradeOrderCost = 0;
            cavalry4.upgradeTime = 60;
            unitDefinitions[14] = cavalry4;

            Unit cavalry5{};
            cavalry5.id = 15;
            cavalry5.trainingTime = 60;
            cavalry5.name = "Cavalry";
            cavalry5.preference = MISSILE;
            cavalry5.unitType = CAVALRY;
            cavalry5.level = 6;
            cavalry5.speed = 6;
            cavalry5.range = 0;
            cavalry5.meleeAttack = 14;
            cavalry5.rangeAttack = 0;
            cavalry5.meleeDefence = 4;
            cavalry5.rangeDefence = 3;
            cavalry5.health = 50;
            cavalry5.bonusDamageCount = 0;
            cavalry5.counterBonus = 0;
            cavalry5.foodCost = 200;
            cavalry5.metalCost = 50;
            cavalry5.upgradeFoodCost = 100;
            cavalry5.upgradeWoodCost = 100;
            cavalry5.upgradeMetalCost = 100;
            cavalry5.upgradeOrderCost = 0;
            cavalry5.upgradeTime = 60;
            unitDefinitions[15] = cavalry5;

            // cavalry.bonusDamage.Add(INFANTRY, 2);

            Unit swordsman0{};
            swordsman0.id = 20;
            swordsman0.trainingTime = 5;
            swordsman0.name = "Swordsman";
            swordsman0.preference = INFANTRY;
            swordsman0.unitType = INFANTRY;
            swordsman0.level = 1;
            swordsman0.speed = 2;        //4 should be standard footspeed for footsoldiers.
            swordsman0.range = 0;
            swordsman0.meleeAttack = 8;
            swordsman0.rangeAttack = 0;
            swordsman0.meleeDefence = 5;
            swordsman0.rangeDefence = 4;
            swordsman0.health = 15;
            swordsman0.bonusDamageCount = 0;
            swordsman0.counterBonus = 0;
            swordsman0.foodCost = 50;
            swordsman0.metalCost = 10;
            swordsman0.upgradeFoodCost = 100;
            swordsman0.upgradeWoodCost = 100;
            swordsman0.upgradeMetalCost = 100;
            swordsman0.upgradeOrderCost = 0;
            swordsman0.upgradeTime = 60;
            unitDefinitions[20] = swordsman0;

            Unit swordsman1{};
            swordsman1.id = 21;
            swordsman1.trainingTime = 5;
            swordsman1.name = "Swordsman";
            swordsman1.preference = INFANTRY;
            swordsman1.unitType = INFANTRY;
            swordsman1.level = 2;
            swordsman1.speed = 2;        //4 should be standard footspeed for footsoldiers.
            swordsman1.range = 0;
            swordsman1.meleeAttack = 8;
            swordsman1.rangeAttack = 0;
            swordsman1.meleeDefence = 5;
            swordsman1.rangeDefence = 4;
            swordsman1.health = 20;
            swordsman1.bonusDamageCount = 0;
            swordsman1.counterBonus = 0;
            swordsman1.foodCost = 50;
            swordsman1.metalCost = 10;
            swordsman1.upgradeFoodCost = 100;
            swordsman1.upgradeWoodCost = 100;
            swordsman1.upgradeMetalCost = 100;
            swordsman1.upgradeOrderCost = 0;
            swordsman1.upgradeTime = 60;
            unitDefinitions[21] = swordsman1;

            Unit swordsman2{};
            swordsman2.id = 22;
            swordsman2.trainingTime = 5;
            swordsman2.name = "Swordsman";
            swordsman2.preference = INFANTRY;
            swordsman2.unitType = INFANTRY;
            swordsman2.level = 3;
            swordsman2.speed = 2;        //4 should be standard footspeed for footsoldiers.
            swordsman2.range = 0;
            swordsman2.meleeAttack = 8;
            swordsman2.rangeAttack = 0;
            swordsman2.meleeDefence = 5;
            swordsman2.rangeDefence = 4;
            swordsman2.health = 15;
            swordsman2.bonusDamageCount = 0;
            swordsman2.counterBonus = 0;
            swordsman2.foodCost = 50;
            swordsman2.metalCost = 10;
            swordsman2.upgradeFoodCost = 100;
            swordsman2.upgradeWoodCost = 100;
            swordsman2.upgradeMetalCost = 100;
            swordsman2.upgradeOrderCost = 0;
            swordsman2.upgradeTime = 60;
            unitDefinitions[22] = swordsman2;

            Unit swordsman3{};
            swordsman3.id = 23;
            swordsman3.trainingTime = 5;
            swordsman3.name = "Swordsman";
            swordsman3.preference = INFANTRY;
            swordsman3.unitType = INFANTRY;
            swordsman3.level = 4;
            swordsman3.speed = 2;        //4 should be standard footspeed for footsoldiers.
            swordsman3.range = 0;
            swordsman3.meleeAttack = 8;
            swordsman3.rangeAttack = 0;
            swordsman3.meleeDefence = 5;
            swordsman3.rangeDefence = 4;
            swordsman3.health = 15;
            swordsman3.bonusDamageCount = 0;
            swordsman3.counterBonus = 0;
            swordsman3.foodCost = 50;
            swordsman3.metalCost = 10;
            swordsman3.upgradeFoodCost = 100;
            swordsman3.upgradeWoodCost = 100;
            swordsman3.upgradeMetalCost = 100;
            swordsman3.upgradeOrderCost = 0;
            swordsman3.upgradeTime = 60;
            unitDefinitions[23] = swordsman3;

            Unit swordsman4{};
            swordsman4.id = 24;
            swordsman4.trainingTime = 5;
            swordsman4.name = "Swordsman";
            swordsman4.preference = INFANTRY;
            swordsman4.unitType = INFANTRY;
            swordsman4.level = 5;
            swordsman4.speed = 2;        //4 should be standard footspeed for footsoldiers.
            swordsman4.range = 0;
            swordsman4.meleeAttack = 8;
            swordsman4.rangeAttack = 0;
            swordsman4.meleeDefence = 5;
            swordsman4.rangeDefence = 4;
            swordsman4.health = 15;
            swordsman4.bonusDamageCount = 0;
            swordsman4.counterBonus = 0;
            swordsman4.foodCost = 50;
            swordsman4.metalCost = 10;
            swordsman4.upgradeFoodCost = 100;
            swordsman4.upgradeWoodCost = 100;
            swordsman4.upgradeMetalCost = 100;
            swordsman4.upgradeOrderCost = 0;
            swordsman4.upgradeTime = 60;
            unitDefinitions[24] = swordsman4;

            Unit swordsman5{};
            swordsman5.id = 25;
            swordsman5.trainingTime = 5;
            swordsman5.name = "Swordsman";
            swordsman5.preference = INFANTRY;
            swordsman5.unitType = INFANTRY;
            swordsman5.level = 6;
            swordsman5.speed = 2;        //4 should be standard footspeed for footsoldiers.
            swordsman5.range = 0;
            swordsman5.meleeAttack = 8;
            swordsman5.rangeAttack = 0;
            swordsman5.meleeDefence = 5;
            swordsman5.rangeDefence = 4;
            swordsman5.health = 15;
            swordsman5.bonusDamageCount = 0;
            swordsman5.counterBonus = 0;
            swordsman5.foodCost = 50;
            swordsman5.metalCost = 10;
            swordsman5.upgradeFoodCost = 100;
            swordsman5.upgradeWoodCost = 100;
            swordsman5.upgradeMetalCost = 100;
            swordsman5.upgradeOrderCost = 0;
            swordsman5.upgradeTime = 60;
            unitDefinitions[25] = swordsman5;

            Unit spearman0{};
            spearman0.id = 30;
            spearman0.trainingTime = 3;
            spearman0.name = "Spearman";
            spearman0.preference = CAVALRY;
            spearman0.unitType = INFANTRY;
            spearman0.level = 1;
            spearman0.speed = 3;
            spearman0.range = 0;
            spearman0.meleeAttack = 7;
            spearman0.rangeAttack = 0;
            spearman0.meleeDefence = 3;
            spearman0.rangeDefence = 2;       //Should probably have less.
            spearman0.health = 12;
            spearman0.bonusDamageCount = 1;
            spearman0.bonusDamageTargets.push_back(CAVALRY);
            spearman0.bonusDamageValues.push_back(10);
            spearman0.counterBonus = 0;
            spearman0.foodCost = 30;
            spearman0.woodCost = 20;
            spearman0.upgradeFoodCost = 100;
            spearman0.upgradeWoodCost = 100;
            spearman0.upgradeMetalCost = 100;
            spearman0.upgradeOrderCost = 0;
            spearman0.upgradeTime = 60;
            unitDefinitions[30] = spearman0;

            Unit spearman1{};
            spearman1.id = 31;
            spearman1.trainingTime = 3;
            spearman1.name = "Spearman";
            spearman1.preference = CAVALRY;
            spearman1.unitType = INFANTRY;
            spearman1.level = 2;
            spearman1.speed = 3;
            spearman1.range = 0;
            spearman1.meleeAttack = 7;
            spearman1.rangeAttack = 0;
            spearman1.meleeDefence = 3;
            spearman1.rangeDefence = 2;       //Should probably have less.
            spearman1.health = 14;
            spearman1.bonusDamageCount = 1;
            spearman1.bonusDamageTargets.push_back(CAVALRY);
            spearman1.bonusDamageValues.push_back(10);
            spearman1.counterBonus = 0;
            spearman1.foodCost = 30;
            spearman1.woodCost = 20;
            spearman1.upgradeFoodCost = 100;
            spearman1.upgradeWoodCost = 100;
            spearman1.upgradeMetalCost = 100;
            spearman1.upgradeOrderCost = 0;
            spearman1.upgradeTime = 60;
            unitDefinitions[31] = spearman1;

            Unit spearman2{};
            spearman2.id = 32;
            spearman2.trainingTime = 3;
            spearman2.name = "Spearman";
            spearman2.preference = CAVALRY;
            spearman2.unitType = INFANTRY;
            spearman2.level = 3;
            spearman2.speed = 3;
            spearman2.range = 0;
            spearman2.meleeAttack = 7;
            spearman2.rangeAttack = 0;
            spearman2.meleeDefence = 3;
            spearman2.rangeDefence = 2;       //Should probably have less.
            spearman2.health = 12;
            spearman2.bonusDamageCount = 1;
            spearman2.bonusDamageTargets.push_back(CAVALRY);
            spearman2.bonusDamageValues.push_back(10);
            spearman2.counterBonus = 0;
            spearman2.foodCost = 30;
            spearman2.woodCost = 20;
            spearman2.upgradeFoodCost = 100;
            spearman2.upgradeWoodCost = 100;
            spearman2.upgradeMetalCost = 100;
            spearman2.upgradeOrderCost = 0;
            spearman2.upgradeTime = 60;
            unitDefinitions[32] = spearman2;

            Unit spearman3{};
            spearman3.id = 33;
            spearman3.trainingTime = 3;
            spearman3.name = "Spearman";
            spearman3.preference = CAVALRY;
            spearman3.unitType = INFANTRY;
            spearman3.level = 4;
            spearman3.speed = 3;
            spearman3.range = 0;
            spearman3.meleeAttack = 7;
            spearman3.rangeAttack = 0;
            spearman3.meleeDefence = 3;
            spearman3.rangeDefence = 2;       //Should probably have less.
            spearman3.health = 12;
            spearman3.bonusDamageCount = 1;
            spearman3.bonusDamageTargets.push_back(CAVALRY);
            spearman3.bonusDamageValues.push_back(10);
            spearman3.counterBonus = 0;
            spearman3.foodCost = 30;
            spearman3.woodCost = 20;
            spearman3.upgradeFoodCost = 100;
            spearman3.upgradeWoodCost = 100;
            spearman3.upgradeMetalCost = 100;
            spearman3.upgradeOrderCost = 0;
            spearman3.upgradeTime = 60;
            unitDefinitions[33] = spearman3;

            Unit spearman4{};
            spearman4.id = 34;
            spearman4.trainingTime = 3;
            spearman4.name = "Spearman";
            spearman4.preference = CAVALRY;
            spearman4.unitType = INFANTRY;
            spearman4.level = 5;
            spearman4.speed = 3;
            spearman4.range = 0;
            spearman4.meleeAttack = 7;
            spearman4.rangeAttack = 0;
            spearman4.meleeDefence = 3;
            spearman4.rangeDefence = 2;       //Should probably have less.
            spearman4.health = 12;
            spearman4.bonusDamageCount = 1;
            spearman4.bonusDamageTargets.push_back(CAVALRY);
            spearman4.bonusDamageValues.push_back(10);
            spearman4.counterBonus = 0;
            spearman4.foodCost = 30;
            spearman4.woodCost = 20;
            spearman4.upgradeFoodCost = 100;
            spearman4.upgradeWoodCost = 100;
            spearman4.upgradeMetalCost = 100;
            spearman4.upgradeOrderCost = 0;
            spearman4.upgradeTime = 60;
            unitDefinitions[34] = spearman4;

            Unit spearman5{};
            spearman5.id = 35;
            spearman5.trainingTime = 3;
            spearman5.name = "Spearman";
            spearman5.preference = CAVALRY;
            spearman5.unitType = INFANTRY;
            spearman5.level = 6;
            spearman5.speed = 3;
            spearman5.range = 0;
            spearman5.meleeAttack = 7;
            spearman5.rangeAttack = 0;
            spearman5.meleeDefence = 3;
            spearman5.rangeDefence = 2;       //Should probably have less.
            spearman5.health = 12;
            spearman5.bonusDamageCount = 1;
            spearman5.bonusDamageTargets.push_back(CAVALRY);
            spearman5.bonusDamageValues.push_back(10);
            spearman5.counterBonus = 0;
            spearman5.foodCost = 30;
            spearman5.woodCost = 20;
            spearman5.upgradeFoodCost = 100;
            spearman5.upgradeWoodCost = 100;
            spearman5.upgradeMetalCost = 100;
            spearman5.upgradeOrderCost = 0;
            spearman5.upgradeTime = 60;
            unitDefinitions[35] = spearman5;
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
        TownBuilding::TownBuilding()
        {
            this->id = 0;
            this->type = TOWNHALL;
            this->buildingTime = 69;
            this->name = "NAMEMISSING";
            this->health = 0;
            this->foodCost = 0;
            this->woodCost = 0;
            this->metalCost = 0;
            this->orderCost = 0;
            this->level = 0;
            this->maxLevel = 0;
        }
}
}