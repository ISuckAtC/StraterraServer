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
#include <iomanip>
#include <fstream>
#include <boost/signals2/signal.hpp>
#include <boost/bind/bind.hpp>

#include "Game.h"
#include "Definition.h"
#include "EventHub.h"
#include "Player.h"
#include "Map.h"
#include "ScheduledEvent.h"

namespace Straterra
{
	namespace Game
	{
		using namespace Straterra::Definition;
		using namespace Straterra::Player;

		static std::vector<User*> users;
		static std::vector<Session*> sessions;
		static std::vector<int> startLocations;
		static int lastStartLocation = 0;
		int userCount;
		int usersOnline;
		int tickInterval;
		int timeOutSeconds;
		std::thread updateThread;
		std::thread slowUpdateThread;
		std::thread autoSaveThread;

		int tickCount = 0;

		void setLastStartIndex(int index)
		{
			lastStartLocation = index;
		}

		void addStartLocation(int position)
		{
			startLocations.push_back(position);
		}

		int getNextStartLocation()
		{
			int location = startLocations[lastStartLocation];
			std::cout << "Gave new player village position: " << location << " (index: " << lastStartLocation << ")" << std::endl;
			lastStartLocation++;
			return location;
		}

		void saveLastStartIndex()
		{
			std::ofstream saveFile{ "lastStartLocation.txt" };

			saveFile << std::to_string(lastStartLocation) << "\0";

			saveFile.close();
		}

		void removeSession(int userId)
		{
			int index = -1;
			for (int i = 0; i < sessions.size(); ++i)
			{
				if (sessions[i]->playerId == userId) index = i;
			}
			if (index > -1)
			{
				sessions.erase(sessions.begin() + index);
			}
		}

		int createUserId()
		{
			int id;
			do
			{
				id = rand();
			} while ((long)getUserById(id) != 0);
			return id;
		}
		long long createSessionToken()
		{
			long long token = 0;
			do
			{
				token = rand();
			} while (findUserBySession(token) != -1);
			return token;
		}

		int getTickInterval()
		{
			return tickInterval;
		}
		int getTicksPerHour()
		{
			return 3600000 / getTickInterval();
		}

		void addUser(User* user)
		{

			//std::cout << "user added: " << user->name;
			users.push_back(user);

			EventHub::subcribeOnTick(boost::bind(&User::addResources, user));
			//std::cout << " | p:" << users[userCount]->name << std::endl;
			//std::cout << "userCount p: " + std::to_string((long)&userCount) << std::endl;

			Map::Tile* homeTile = Map::getTile(user->cityLocation);
			homeTile->building = 1;
			homeTile->owner = user->userId;

			userCount++;

			std::cout << "Added user " << std::to_string(user->userId) << std::endl;
		}
		void addSession(Session* session)
		{
			sessions.insert(sessions.begin(), session);
			usersOnline++;
		}





		User* getUserById(int id)
		{
			std::cout << "gI";
			int index = -1;
			for (int i = 0; i < userCount; ++i)
			{
				//std::cout << users[i]->userId << std::endl;
				if (users[i]->userId == id)
				{
					return users[i];
				}
			}
			std::cout << "getUser: user not found (id: " << id << ")" << std::endl;
			return (User*)0;
		}

		User* getUserAt(int index)
		{
			//std::cout << "getUserAt: " << std::to_string((long)users[index]) << std::endl;
			return users[index];
		}

		bool getUserOnline(int id)
		{
			for (int i = 0; i < sessions.size(); ++i)
			{
				if (sessions[i]->playerId == id) return true;
			}
			return false;
		}

		User* getUserByEmail(std::string email)
		{
			for (int i = 0; i < userCount; ++i)
			{
				if (users[i]->email == email)
				{
					return users[i];
				}
			}
			return (User*)0;
		}

		User* getUserByName(std::string name) 
		{
			for (int i = 0; i < userCount; ++i)
			{
				if (users[i]->name == name)
				{
					return users[i];
				}
			}
			return (User*)0;
		}

		User* getUserBySession(long long token)
		{
			for (int i = 0; i < usersOnline; ++i)
			{
				if (sessions[i]->token == token)
				{
					time_t now;
					time(&now);
					sessions[i]->lastSeen = now;
					return getUserById(sessions[i]->playerId);
				}
			}
			return (User*)0;
		}

		int findUserBySession(long long token)
		{
			std::cout << "fS";
			int id = -1;
			//std::cout << "usersOnline: " << std::to_string(usersOnline) << std::endl;
			//std::cout << token << std::endl;
			for (int i = 0; i < usersOnline; ++i)
			{
				//std::cout << sessions[i]->token << std::endl;
				if (sessions[i]->token == token)
				{
					time_t now;
					time(&now);
					sessions[i]->lastSeen = now;
					id = sessions[i]->playerId;
				}
			}
			return id;
		}

		std::string Session::tokenBytes()
		{
			return getTokenString(token);
		}

		std::string getTokenString(long long token)
		{
			return std::to_string(token);
			//std::cout << "getTokenString: " << std::to_string(token) << std::endl;
			std::stringstream ss;
			ss << std::hex;

			for (int i = 0; i < sizeof(long long); ++i)
			{
				uint8_t byte = *((uint8_t*)(&token + i));
				//std::cout << "|" << std::to_string(byte);
				ss << std::setw(2) << std::setfill('0') << (int)byte;
			}
			//std::cout << std::endl << ss.str() << std::endl;
			return ss.str();
		}

		long long getTokenLong(std::string tokenBytes)
		{
			return std::stoll(tokenBytes);
			long long token = 0;
			uint8_t bytes[8];
			for (int i = 0; i < 16; i += 2)
			{
				std::string part = tokenBytes.substr(i, 2);
				token |= (uint8_t)strtol(part.c_str(), NULL, 16) << (i * 4);
			}
			return token;
		}

		void update()
		{
			try
			{
				while (running)
				{
					// update logic
					std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::milliseconds(tickInterval));
					EventHub::fireOnTick();
					tickCount++;
				}
			}
			catch (std::exception const& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
		void slowUpdate()
		{
			try
			{
				while (running)
				{
					// slow update logic
					std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::milliseconds(tickInterval * 30));
					time_t now;
					tm timeNow;
					time(&now);
					localtime_s(&timeNow, &now);
					char timeChars[32];
					std::strftime(timeChars, 32, "%a, %d.%m.%Y %H:%M:%S", &timeNow);
					std::cout << timeChars << std::endl;
					for (int i = 0; i < usersOnline; ++i)
					{
						if (sessions[i]->playerId == 1984) std::cout << "Diff: " << std::difftime(now, sessions[i]->lastSeen) << std::endl;
						if (std::difftime(now, sessions[i]->lastSeen) > timeOutSeconds)
						{
							sessions.erase(sessions.begin() + i);
							usersOnline--;
							i--;
						}
					}
				}
			}
			catch (std::exception const& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}

		std::string saveSerialUser(User user)
		{
			std::ostringstream stream{};
			stream <<
				user.userId << ";" <<
				user.login << ";" <<
				user.name << ";" <<
				user.cityLocation << ";" <<
				user.color << ";" <<
				user.allianceId << ";" <<
				user.food << ";" <<
				user.wood << ";" <<
				user.metal << ";" <<
				user.order << ";" <<
				user.foodGeneration << ";" <<
				user.woodGeneration << ";" <<
				user.metalGeneration << ";" <<
				user.orderGeneration << ";";

			for (int i = 0; i < 8; ++i)
			{
				stream << std::to_string((int)user.cityBuildingSlots[i]) << ";";
			}

			stream << 
				std::to_string(user.archerLevel) << ";" <<
				std::to_string(user.cavalryLevel) << ";" <<
				std::to_string(user.swordLevel) << ";" <<
				std::to_string(user.spearmanLevel) << ";";

			stream <<
				std::to_string(user.cityUpgradeCap) << ";" <<
				std::to_string(user.populationCap) << ";" <<
				std::to_string(user.population) << ";";

			return stream.str();
		}

		void autoSave()
		{
			try
			{
				std::vector<User> saveUsers;
				int saveUserCount = 0;
				while (running)
				{
					std::this_thread::sleep_until(std::chrono::steady_clock::now() + std::chrono::milliseconds(tickInterval * 1800));
					saveUserCount = 0;

					// Save current users to its own storage
					for (int i = 0; i < users.size(); ++i)
					{
						saveUsers.push_back(*users[i]);
						saveUserCount++;
					}



					std::ofstream saveFile{ "players.txt" };
					std::string line;

					for (int i = 0; i < saveUserCount; ++i)
					{
						saveFile << saveSerialUser(saveUsers[i]) << std::endl;
					}
					saveFile << "\0";

					saveFile.close();

					saveLastStartIndex();

					std::cout << "Autosaved" << std::endl;

					saveUsers.clear();
				}
				
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}
			
		}



		void start(int _tickInterval, int _timeOutSeconds)
		{
			userCount = 0;
			usersOnline = 0;
			tickInterval = _tickInterval;
			timeOutSeconds = _timeOutSeconds;
			// start logic
			updateThread = std::thread{ update };
			slowUpdateThread = std::thread{ slowUpdate };
			autoSaveThread = std::thread{ autoSave };
			std::cout << "Game Started" << std::endl;
		}


		int getUserCount()
		{
			//std::cout << "getUserCount: " << userCount << " real count: " << users.size() << std::endl;
			//std::cout << "userCount p: " + std::to_string((long)&userCount) << std::endl;
			return userCount;
		}

		int getUserOnlineCount()
		{
			return usersOnline;
		}

		int Group::TakeDamage(int damage, Group* source, bool melee, bool counterable, bool verbose, bool trample)
		{
			Unit unit = getUnitDefinition(unitId);
			Unit enemyUnit = getUnitDefinition(source->unitId);

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



		Session::Session(int playerId, long long token)
		{
			this->playerId = playerId;
			this->token = token;
			time_t now;
			time(&now);
			this->lastSeen = now;
		}

		int Group::GetDamage(int range, Group* enemy, bool counter, bool trampleCounter)
		{
			Unit unit = getUnitDefinition(unitId);
			Unit enemyUnit = getUnitDefinition(enemy->unitId);

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

		Group::Group(int _count, int _unitId)
		{
			count = _count;
			unitId = _unitId;
			frontHealth = getUnitDefinition(unitId).health;
			position = 0;
			target = -1;
			right = false;
			dead = false;
		}

		bool Group::operator==(Group other)
		{
			return unitId == other.unitId;
		}



		bool Fight(std::vector<Group>* unitsLeft, std::vector<Group>* loserUnitsLeft, std::string* output, std::vector<Group> defender_, std::vector<Group> attacker_, bool city, bool verbose)
		{
			std::vector<Group*> attacker;
			std::vector<Group*> defender;

			bool attackerRetreating = false;
			bool defenderRetreating = false;

			int attackerRetreatThreshold = 0;
			int defenderRetreatThreshold = 0;

			int attackerRetreatThresholdPlus = 0;
			int defenderRetreatThresholdPlus = 0;

			int attackerCount = 0;
			int defenderCount = 0;


			for (int i = 0; i < attacker_.size(); ++i)
			{
				attackerRetreatThreshold += attacker_[i].count;
				attacker.push_back(new Group(attacker_[i]));
			}
			for (int i = 0; i < defender_.size(); ++i)
			{
				defenderRetreatThreshold += defender_[i].count;
				defender.push_back(new Group(defender_[i]));
			}

			attackerRetreatThresholdPlus = attackerRetreatThreshold / 7;
			defenderRetreatThresholdPlus = defenderRetreatThreshold / 7;

			attackerRetreatThreshold /= 10;
			defenderRetreatThreshold /= 10;

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
				int range = getUnitDefinition(defender[i]->unitId).range;
				if (range > leftRange) leftRange = range;
			}

			int rightRange = 0;
			for (int i = 0; i < attacker.size(); ++i)
			{
				int range = getUnitDefinition(attacker[i]->unitId).range;
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
				Unit unit = getUnitDefinition(defender[i]->unitId);
				*output += std::to_string(unit.id) + " LV" + std::to_string(unit.level) + " (" + std::to_string(defender[i]->count) + ")\n";
			}
			*output += "Red Team\n";
			for (int i = 0; i < attacker.size(); ++i)
			{
				Unit unit = getUnitDefinition(attacker[i]->unitId);
				*output += std::to_string(unit.id) + " LV" + std::to_string(unit.level) + " (" + std::to_string(attacker[i]->count) + ")\n";
			}



			std::sort(all.begin(), all.end(),
				[](const Group* a, const Group* b) -> bool
				{
					return getUnitDefinition(a->unitId).speed + (a->right ? 0.5f : 0) > getUnitDefinition(b->unitId).speed + (b->right ? 0.5f : 0);
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
			int turn = all.size();
			int totalTurns = 0;



			while (combat)
			{
				totalTurns++;
				//Console.ReadLine();

				if (turn == all.size())
				{
					attackerCount = 0;
					defenderCount = 0;
					for (int i = 0; i < attacker.size(); ++i)
					{
						if (!attacker[i]->dead) attackerCount += attacker[i]->count;
					}
					for (int i = 0; i < defender.size(); ++i)
					{
						if (!defender[i]->dead) defenderCount += defender[i]->count;
					}

					attackerRetreating = attackerCount < attackerRetreatThreshold&& defenderCount > defenderRetreatThresholdPlus;
					defenderRetreating = defenderCount < defenderRetreatThreshold&& attackerCount > attackerRetreatThresholdPlus && !city;


					turn = 0;
				}

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
						" | Unit Type: " << getUnitDefinition(group->unitId).id <<
						" | Count: " << group->count <<
						" | Front Health: " << group->frontHealth <<
						" | Position: " << group->position <<
						std::endl;
				}

				//std::cout << group->target << std::endl;
				//std::cout << 

				if ((group->right ? attackerRetreating : defenderRetreating))
				{
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
					std::reverse(aliveTargets.begin(), aliveTargets.end());

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
								Unit unit = getUnitDefinition(winningTeam[i]->unitId);
								*output += std::to_string(unit.id) +
									" LV" + std::to_string(unit.level) +
									" (" + std::to_string(remains[0].count) + ")\n";
							}
						}
						*unitsLeft = remains;
						return group->right;
					}
					// 50 is abritrary
					// TODO: Make the escape range based off of something
					if (std::abs(group->position - aliveTargets[0]->position) >= 50)
					{
						// Escape

						if (verbose) std::cout << (group->right ? "Right escapes" : "Left escapes") << " in " << totalTurns << " turns!" << std::endl;

						*output += "\n\nWinning Team: " + std::string(group->right ? "Left" : "Right") + "\nRemaining Units\n";

						std::vector<Group*> winningTeam = (!group->right ? attacker : defender);
						std::vector<Group> remains;
						for (int i = 0; i < winningTeam.size(); ++i)
						{
							if (!winningTeam[i]->dead)
							{
								std::vector<Group>::iterator itnp = remains.begin();
								remains.insert(itnp, *winningTeam[i]);
								Unit unit = getUnitDefinition(winningTeam[i]->unitId);
								*output += std::to_string(unit.id) +
									" LV" + std::to_string(unit.level) +
									" (" + std::to_string(remains[0].count) + ")\n";
							}
						}

						std::vector<Group*> losingTeam = (!group->right ? defender : attacker);
						std::vector<Group> loserRemains;

						for (int i = 0; i < losingTeam.size(); ++i)
						{
							if (!losingTeam[i]->dead)
							{
								std::vector<Group>::iterator itnp = loserRemains.begin();
								loserRemains.insert(itnp, *losingTeam[i]);
								Unit unit = getUnitDefinition(losingTeam[i]->unitId);
								*output += std::to_string(unit.id) +
									" LV" + std::to_string(unit.level) +
									" (" + std::to_string(loserRemains[0].count) + ")\n";
							}
						}

						*unitsLeft = remains;
						*loserUnitsLeft = loserRemains;
						return !group->right;
					}
					int moveSpeed = getUnitDefinition(group->unitId).speed;
					group->position += (group->right ? moveSpeed : -moveSpeed);
				}
				else
				{
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
							if (getUnitDefinition(aliveTargets[i]->unitId).unitType == getUnitDefinition(group->unitId).preference)
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
									Unit unit = getUnitDefinition(winningTeam[i]->unitId);
									*output += std::to_string(unit.id) +
										" LV" + std::to_string(unit.level) +
										" (" + std::to_string(remains[0].count) + ")\n";
								}
							}
							*unitsLeft = remains;
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

					if (distance > getUnitDefinition(group->unitId).range)
					{
						int move = getUnitDefinition(group->unitId).speed;
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
						if (getUnitDefinition(group->unitId).unitType == CAVALRY)
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

					if (distance <= getUnitDefinition(group->unitId).range)
					{
						// Attack
						int damage = group->GetDamage(distance, enemy);

						if (verbose) std::cout << group->unitId << " attacks " << enemy->unitId << " for " << damage << " damage!" << std::endl;

						int deaths = enemy->TakeDamage(damage, group, distance == 0, true, verbose);

						if (verbose) std::cout << "Attack caused " << deaths << " deaths" << std::endl;
					}
				}

				turn++;
			}
			std::cout << "Something wrong happened in fight, returning default." << std::endl;
			return false;
		}
	}
}