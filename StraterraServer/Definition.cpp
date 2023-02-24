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
		Unit getUnitDefinition(int i)
		{
			return unitDefinitions[i];
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
	}
}