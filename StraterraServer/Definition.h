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

			int GetBonusDamage(int targetType);
		};

		Unit getUnitDefinition(int i);

		void DefineUnits();
	}
}
#endif