// StraterraServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

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
#include "Http.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//using namespace Straterra;

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

			int GetBonusDamage(int targetType)
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
		};

		Unit unitDefinitions[256];

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
	namespace Game
	{
		using namespace Straterra::Definition;
		int playerCount = 0;
		int playersOnline = 0;

		class Player
		{
		public:
			int playerId;
			int cityLocation;
			BYTE cityBuildingSlots[8];
			BYTE swordLevel;
			BYTE archerLevel;
			BYTE cavalryLevel;
			BYTE spearmanLevel;

		private:

		};

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
			Group(int _count, int _unitId)
			{
				count = _count;
				unitId = _unitId;
				frontHealth = unitDefinitions[unitId].health;
				position = 0;
				target = -1;
				right = false;
				dead = false;
			}

			bool operator==(Group other)
			{
				return unitId == other.unitId;
			}

			int GetDamage(int range, Group* enemy, bool counter = false, bool trampleCounter = false)
			{
				Unit unit = unitDefinitions[unitId];
				Unit enemyUnit = unitDefinitions[enemy->unitId];

				int damage = 0;

				int attackCount = count;

				if (range > 0)
				{
					// Ranged
					damage += unit.rangeAttack;
					damage -= enemyUnit.rangeDefence;
				}
				else
				{
					// Melee
					damage += unit.meleeAttack;
					if (counter) damage += unit.counterBonus;
					damage -= enemyUnit.meleeDefence;

				}

				if (trampleCounter && damage > 1) damage = 1;

				damage += unit.GetBonusDamage(enemyUnit.unitType);

				if (range == 0)
				{
					if (attackCount > (enemy->count * 2.5)) attackCount = (int)(enemy->count * 2.5);
				}

				if (damage < 1) damage = 1;


				return damage * attackCount;
			}

			int TakeDamage(int damage, Group* source, bool melee, bool counterable = true, bool verbose = true, bool trample = false)
			{
				Unit unit = unitDefinitions[unitId];
				Unit enemyUnit = unitDefinitions[source->unitId];

				int deaths = damage / unit.health;
				int rest = damage % unit.health;
				frontHealth -= rest;
				if (frontHealth <= 0)
				{
					deaths++;
					frontHealth = unit.health + frontHealth;
				}

				if (melee && counterable)
				{
					int counterDamage = GetDamage(0, source, true, trample);
					if (verbose) std::cout << unit.id << " counters " << enemyUnit.id << " for " << counterDamage << " damage!" << std::endl;

					int cDeaths = source->TakeDamage(counterDamage, this, true, false, verbose);

					if (verbose) std::cout << "Counter caused " << cDeaths << " deaths" << std::endl;
				}

				count -= deaths;

				if (count < 1)
				{
					deaths += count;

					dead = true;
				}

				return deaths;
			}
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



		bool Fight(std::vector<Group>* unitsLeft, std::string* output, std::vector<Group*> defender, std::vector<Group*> attacker, bool verbose = false)
		{
			std::sort(defender.begin(), defender.end(),
				[](const Group* a, const Group* b) -> bool
				{
					return a->unitId < b->unitId;
				});
			std::sort(attacker.begin(), attacker.end(),
				[](const Group* a, const Group* b) -> bool
				{
					return a->unitId < b->unitId;
				});


			int leftRange = 0;
			for (int i = 0; i < defender.size(); ++i)
			{
				int range = unitDefinitions[defender[i]->unitId].range;
				if (range > leftRange) leftRange = range;
			}

			int rightRange = 0;
			for (int i = 0; i < attacker.size(); ++i)
			{
				int range = unitDefinitions[attacker[i]->unitId].range;
				if (range > rightRange) rightRange = range;
			}


			for (int i = 0; i < defender.size(); ++i)
			{
				defender[i]->position = -leftRange;
				defender[i]->right = false;
				//std::cout << defender[i] << " | " << defender[i]->position << " | " << defender[i]->dead << " | " << defender[i]->right << std::endl;
			}

			for (int i = 0; i < attacker.size(); ++i)
			{
				attacker[i]->position = rightRange;
				attacker[i]->right = true;
				//std::cout << attacker[i] << " | " << attacker[i]->position << " | " << attacker[i]->dead << " | " << attacker[i]->right << std::endl;
			}

			

			std::vector<Group*> all;
			std::vector<Group*>::iterator it = all.begin();
			all.insert(it, defender.begin(), defender.end());
			it = all.begin();
			all.insert(it, attacker.begin(), attacker.end());

			*output = "";
			*output += "Green Team\n";
			for (int i = 0; i < defender.size(); ++i)
			{
				Unit unit = unitDefinitions[defender[i]->unitId];
				*output += std::to_string(unit.id) + " LV" + std::to_string(unit.level) + " (" + std::to_string(defender[i]->count) + ")\n";
			}
			*output += "Red Team\n";
			for (int i = 0; i < attacker.size(); ++i)
			{
				Unit unit = unitDefinitions[attacker[i]->unitId];
				*output += std::to_string(unit.id) + " LV" + std::to_string(unit.level) + " (" + std::to_string(attacker[i]->count) + ")\n";
			}

			

			std::sort(all.begin(), all.end(),
				[](const Group* a, const Group* b) -> bool
				{
					return unitDefinitions[a->unitId].speed + (a->right ? 0.5f : 0) > unitDefinitions[b->unitId].speed + (b->right ? 0.5f : 0);
				});

			if (verbose)
			{
				std::cout << "Fight starting..." << std::endl << "Participants:" << std::endl;
				for (int i = 0; i < all.size(); ++i)
				{
					std::cout << all[i] << " | " << all[i]->position << " | " << all[i]->dead << " | " << all[i]->right << std::endl;
					std::cout << "UnitId: " << all[i]->unitId << " | Count: " << all[i]->count << " | Team: " << (all[i]->right ? "Attacker" : "Defender") << std::endl;
				}
			}

			bool combat = true;
			int turn = 0;
			int totalTurns = 0;

			

			while (combat)
			{
				totalTurns++;
				//Console.ReadLine();

				if (turn == all.size()) turn = 0;

				Group* group = all[turn];
				if (group->dead)
				{
					std::cout << "Turn index " << turn << " is dead, skipping." << std::endl;
					turn++;
					continue;
				}

				std::vector<Group*> enemyArmy = (group->right ? defender : attacker);


				if (verbose)
				{
					std::cout << "__________________________________________________" << std::endl;
					std::cout << "__________________________________________________" << std::endl;
					std::cout <<
						"[" << totalTurns << "]" << "Turn start: Team: " << (group->right ? 2 : 1) <<
						" | UnitId: " << group->unitId <<
						" | Unit Type: " << unitDefinitions[group->unitId].id <<
						" | Count: " << group->count <<
						" | Front Health: " << group->frontHealth <<
						" | Position: " << group->position <<
						std::endl;
				}

				//std::cout << group->target << std::endl;
				//std::cout << 

				if (group->target < 0 || enemyArmy[group->target]->dead)
				{
					int index = -1;
					std::vector<Group*> aliveTargets;
					for (int i = 0; i < enemyArmy.size(); ++i)
					{
						if (!enemyArmy[i]->dead)
						{
							it = aliveTargets.begin();
							aliveTargets.insert(it, enemyArmy[i]);
						}
					}
					std::sort(aliveTargets.begin(), aliveTargets.end(),
						[group](const Group* a, const Group* b) -> bool
						{
							return std::abs(group->position - a->position) > std::abs(group->position - b->position);
						});
					std::vector<Group*> preferredTargets;
					for (int i = 0; i < aliveTargets.size(); ++i)
					{
						if (unitDefinitions[aliveTargets[i]->unitId].unitType == unitDefinitions[group->unitId].preference)
						{
							it = preferredTargets.begin();
							preferredTargets.insert(it, aliveTargets[i]);
						}
					}
					std::reverse(preferredTargets.begin(), preferredTargets.end());

					//std::cout << "EnemyArmyCount: " << enemyArmy.size() << " | AliveTargetCount: " << aliveTargets.size() << std::endl;

					if (aliveTargets.size() == 0)
					{
						// Winner
						if (verbose) std::cout << (group->right ? "Right wins" : "Left wins") << " in " << totalTurns << " turns!" << std::endl;

						*output += "\n\nWinning Team: " + std::string(group->right ? "Red" : "Green") + "\nRemaining Units\n";

						std::vector<Group*> winningTeam = (group->right ? attacker : defender);
						std::vector<Group> remains;
						for (int i = 0; i < winningTeam.size(); ++i)
						{
							if (!winningTeam[i]->dead)
							{
								std::vector<Group>::iterator itnp = remains.begin();
								remains.insert(itnp, *winningTeam[i]);
								Unit unit = unitDefinitions[winningTeam[i]->unitId];
								*output += std::to_string(unit.id) +
									" LV" + std::to_string(unit.level) +
									" (" + std::to_string(remains[0].count) + ")\n";
							}
						}
						unitsLeft = &remains;
						return group->right;
					}
					if (preferredTargets.size() > 0)
					{
						for (int i = 0; i < enemyArmy.size(); ++i)
						{
							if (enemyArmy[i] == preferredTargets[0])
							{
								index = i;
								break;
							}
						}
					}
					else
					{
						for (int i = 0; i < enemyArmy.size(); ++i)
						{
							if (enemyArmy[i] == aliveTargets[0])
							{
								index = i;
								break;
							}
						}
					}

					group->target = index;

					if (verbose) std::cout << group->unitId << " chose target " << enemyArmy[group->target]->unitId << std::endl;
				}

				Group* enemy = enemyArmy[group->target];

				int distance = std::abs(enemy->position - group->position);

				if (distance > unitDefinitions[group->unitId].range)
				{
					int move = unitDefinitions[group->unitId].speed;
					if (move >= distance)
					{
						move = distance;
						distance = 0;
					}
					int direction = 1;
					if (group->position > enemy->position)
					{
						direction = -1;
					}
					if (unitDefinitions[group->unitId].unitType == CAVALRY)
					{
						// Trample
						for (int i = 0; i < move; ++i)
						{
							std::vector<Group*> tramples;
							for (int j = 0; j < enemyArmy.size(); ++j)
							{
								if (enemyArmy[j]->position == (group->position + (j * direction)))
								{
									it = tramples.begin();
									tramples.insert(it, enemyArmy[j]);
								}
							}
							for (int k = 0; k < tramples.size(); ++k)
							{
								Group* trampleEnemy = tramples[k];
								// Attack
								int damage = group->GetDamage(distance, trampleEnemy) / 5;

								if (verbose) std::cout << group->unitId << " tramples " << trampleEnemy->unitId << " for " << damage << " damage!" << std::endl;

								int deaths = trampleEnemy->TakeDamage(damage, group, true, verbose, true);

								if (verbose) std::cout << "Trample caused " << deaths << " deaths" << std::endl;
							}
						}
					}


					group->position += move * direction;

					if (verbose) std::cout << group->unitId << " moves " << move << " units" << std::endl;
				}

				if (distance <= unitDefinitions[group->unitId].range)
				{
					// Attack
					int damage = group->GetDamage(distance, enemy);

					if (verbose) std::cout << group->unitId << " attacks " << enemy->unitId << " for " << damage << " damage!" << std::endl;

					int deaths = enemy->TakeDamage(damage, group, distance == 0, true, verbose);

					if (verbose) std::cout << "Attack caused " << deaths << " deaths" << std::endl;
				}

				turn++;
			}
			std::cout << "Something wrong happened in fight, returning default." << std::endl;
			return false;
		}


	}
}

using namespace Straterra::Game;


int main()
{
	Straterra::Definition::DefineUnits();
	std::vector<Group*> groupA;
	std::vector<Group*> groupB;

	Group archersA = { 20, 0 };
	Group swordsA = { 20, 2 };

	Group swordsB = { 20, 2 };
	Group archersB = { 20, 0 };

	std::vector<Group*>::iterator it;
	it = groupA.begin();
	groupA.insert(it, &archersA);
	it = groupA.begin();
	groupA.insert(it, &swordsA);

	it = groupB.begin();
	groupB.insert(it, &swordsB);
	it = groupB.begin();
	groupB.insert(it, &archersB);

	std::string output;
	std::vector<Group> remains;
	Fight(&remains, &output, groupA, groupB, true);

	std::cout << output << std::endl;

	

	try
	{
		auto const address = net::ip::make_address_v4("0.0.0.0");
		unsigned short port = 6969;

		net::io_context ioc{ 1 };

		Straterra::Http::http_connection a{ tcp::socket{ioc} };

		tcp::acceptor acceptor{ ioc, {address, port} };
		tcp::socket socket{ ioc };

		Straterra::Http::http_server(acceptor, socket);

		ioc.run();
		return 0;
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
}

