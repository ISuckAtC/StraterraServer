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
#include <boost/signals2/signal.hpp>
#include <boost/bind/bind.hpp>

#include "ScheduledEvent.h"
#include "Game.h"
#include "Player.h"
#include "EventHub.h"
#include "Map.h"
#include "Definition.h"

namespace Straterra
{
	namespace ScheduledEvents
	{
		int* testP = 0;
		int* getTestP()
		{
			if (testP == 0)
			{
				int a = 99999;
				return &a;
			}
			return testP;
		}
		ScheduledEvent::ScheduledEvent(int secondsTotal, int owner, bool runImmediately)
		{
			this->secondsTotal = secondsTotal;
			this->secondsLeft = secondsTotal;
			testP = &(this->secondsLeft);
			//std::cout << "CONSTRUCTOR VALUE: " << (long)(&(this->secondsLeft)) << std::endl;
			this->owner = owner;
			this->ownerEvents = &(Game::getUserById(owner)->activeEvents);
			this->type = GENERIC;
			if (runImmediately)
			{
				//std::cout << "Creating scheduled event" << std::endl;
				Run();
				this->running = true;
			}
			else
			{
				this->running = false;
			}
			this->ownerEvents->insert(this->ownerEvents->begin(), this);
			//std::cout << "CONSTRUCTOR DONE" << std::endl;
		}

		ScheduledEvent::~ScheduledEvent()
		{
			//std::cout << "SCHEDULEDEVENT DECONSTRUCTED" << std::endl;
		}

		void ScheduledEvent::Run()
		{
			running = true;
			subcription = EventHub::subcribeOnTick(boost::bind(&ScheduledEvent::Tick, this));
		}

		void ScheduledEvent::Tick()
		{
			//std::cout << std::to_string((long)this) << " had Tick called (ticks left: " << this->secondsLeft << ")" << std::endl;
			//std::cout << "data on tick: " << this->owner << " | " << this->secondsTotal << " | " << this->secondsLeft << " | " << this->type << std::endl;
			if (secondsLeft-- == 0) Complete();
		}

		void ScheduledEvent::Complete()
		{
			// unsubcribe to tick event here
			// 
			// find iterator for own position and remove it from the vector

			std::cout << "COMPLETE" << std::endl;
			EventHub::unsubcribeOnTick(&subcription);
			ownerEvents->erase(std::find(ownerEvents->begin(), ownerEvents->end(), this));
			//delete this;
		}

		ScheduledUnitProductionEvent::ScheduledUnitProductionEvent(int secondsTotal, int unitId, int amount, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->unitId = unitId;
			this->amount = amount;
			this->type = UNITPRODUCTION;
			//std::cout << "UNIT POINTER: " << (long)this << "| UNITPRODTIMEPOINTER: " << (long)(&(this->secondsLeft)) << "| UNITPRODTIME: " << this->secondsLeft << std::endl;
		}

		void ScheduledUnitProductionEvent::Complete()
		{
			ScheduledEvent::Complete();
			std::vector<ScheduledUnitProductionEvent*> prodEvents;
			//std::copy_if(ownerEvents->begin(), ownerEvents->end(), prodEvents, [&](ScheduledEvent* e) {
			//	return ((dynamic_cast<ScheduledUnitProductionEvent*>(e) != nullptr) && !e->running);
			//	});
			if (prodEvents.size() > 0)
			{
				prodEvents[0]->Run();
			}
			Game::getUserById(owner)->homeArmy[unitId] += amount;
			std::cout << "Added " << amount << " " << unitId << " to army! (Total: " << std::to_string(Game::getUserById(owner)->homeArmy[unitId]) << ")" << std::endl;
			delete this;
		}
		ScheduledTownBuildingEvent::ScheduledTownBuildingEvent(int secondsTotal, int buildingId, int buildingSlot, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			Game::getUserById(owner)->cityBuildingSlots[buildingSlot] = 254;
			this->buildingId = buildingId;
			this->buildingSlot = buildingSlot;
			this->type = TOWNBUILDING;
		}
		void ScheduledTownBuildingEvent::Complete()
		{
			ScheduledEvent::Complete();
			Player::User* user = Game::getUserById(owner);

			EventHub::Report* report = EventHub::Report::CreateReport("Town Building Complete", "Add description here");
			user->reports.push_back(report);

			user->cityBuildingSlots[buildingSlot] = buildingId;
			delete this;
		}

		ScheduledMapBuildingEvent::ScheduledMapBuildingEvent(int secondsTotal, int buildingId, int position, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->buildingId = buildingId;
			this->position = position;
			Map::Tile* tile = Map::getTile(position);
			tile->building = 255;
			tile->owner = owner;
			Player::User* user = Game::getUserById(owner);

			user->mapBuildings.push_back(position);

			this->type = MAPBUILDING;
		}
		void ScheduledMapBuildingEvent::Complete()
		{
			ScheduledEvent::Complete();
			Map::Tile* tile = Map::getTile(position);
			Player::User* user = Game::getUserById(owner);

			Definition::MapBuilding mapBuilding = Definition::getMapBuildingDefinition(buildingId);
			if (mapBuilding.type == Definition::FARM)
			{
				int foodProd = mapBuilding.baseProduction * tile->foodAmount;
				user->foodGeneration += foodProd;
			}
			if (mapBuilding.type == Definition::WOOD)
			{
				int woodProd = mapBuilding.baseProduction * tile->woodAmount;
				user->woodGeneration += woodProd;
			}
			if (mapBuilding.type == Definition::MINE)
			{
				int metalProd = mapBuilding.baseProduction * tile->metalAmount;
				user->foodGeneration += metalProd;
			}

			tile->building = buildingId;

			EventHub::Report* report = EventHub::Report::CreateReport("Map Building Complete", "Add description here");
			user->reports.push_back(report);

			delete this;
		}
		
		ScheduledMoveArmyEvent::ScheduledMoveArmyEvent(int secondsTotal, std::vector<Game::Group> army, int destination, int origin, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->origin = origin;
			this->destination = destination;
			this->army = army;
			this->type = MOVEARMY;
		}
		void ScheduledMoveArmyEvent::Complete()
		{
			ScheduledEvent::Complete();

			Player::User* armyOwner = Game::getUserById(Map::getTile(destination)->owner);

			EventHub::Report* report = EventHub::Report::CreateReport("Army has arrived at " + std::to_string(destination), "Add description here");
			armyOwner->reports.push_back(report);

			if (Map::getTile(destination)->building == 0)
			{
				if (Map::getTile(origin)->building == 1)
				{
					new ScheduledMoveArmyEvent(0, army, origin, destination, owner);
					return;
				}
				delete this;
				return;
			}

			Definition::MapBuilding building = Definition::getMapBuildingDefinition(Map::getTile(destination)->building);

			if (building.type == Definition::VILLAGE)
			{
				for (int i = 0; i < army.size(); ++i)
				{
					
					armyOwner->homeArmy[army[i].unitId] += army[i].count;
				}
				delete this;
				return;
			}

			//if (Grid._instance.tiles[destination].army != null && Grid._instance.tiles[destination].army.Count > 0)
			//{
			//	Debug.Log("Tried to move troops to a tile with troops on it");
			//	return;
			//}
			//Grid._instance.tiles[destination].army = army;
			delete this;
		}
		ScheduledAttackEvent::ScheduledAttackEvent(int secondsTotal, std::vector<Game::Group> army, int destination, int origin, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->origin = origin;
			this->destination = destination;
			this->army = army;
			this->type = ATTACK;

			Map::Tile* targetTile = Map::getTile(destination);

			if (targetTile->building == 1)
			{
				Player::User* attacker = Game::getUserById(owner);
				Player::User* defender = Game::getUserById(targetTile->owner);

				EventHub::Report* defenceWarning = EventHub::Report::CreateReport("Incoming attack!", "You are being attacked by " + attacker->name + ", the attack will arrive in " + std::to_string(secondsTotal) + " seconds.");

				defender->reports.push_back(defenceWarning);
			}

		}
		void ScheduledAttackEvent::Complete()
		{
			ScheduledEvent::Complete();

			std::cout << "Commencing attack on position: " << destination << std::endl;

			Map::Tile* targetTile = Map::getTile(destination);

			Player::User* defendingPlayer = Game::getUserById(targetTile->owner);

			Player::User* attackingPlayer = Game::getUserById(owner);

			std::vector<Game::Group> defender;

			bool defenderCity = false;

			if (targetTile->building == 1 && defendingPlayer->cityLocation == targetTile->id)
			{
				defenderCity = true;
				for (int i = 0; i < 256; ++i)
				{
					if (defendingPlayer->homeArmy[i] > 0) defender.push_back(Game::Group(defendingPlayer->homeArmy[i], i));
				}
			}
			else
			{
				defender = targetTile->army;
			}

			std::vector<Game::Group> unitsLeft;
			std::vector<Game::Group> loserUnitsLeft;
			std::string output;


			bool attackWin = Game::Fight(&unitsLeft, &loserUnitsLeft, &output, defender, army, defenderCity);

			std::cout << (attackWin ? "Attacker Won" : "Defender Won") << " | " << unitsLeft.size() << " groups are left alive" << std::endl;

			std::stringstream reportContent;
			std::stringstream winnerContent;
			std::stringstream loserContent;

			reportContent << "Battle between " <<
				attackingPlayer->name << " (" << attackingPlayer->userId << ")" <<
				" and " <<
				defendingPlayer->name << " (" << defendingPlayer->userId << ")" <<
				std::endl <<
				(attackWin ? attackingPlayer->name : defendingPlayer->name) <<
				" won!" << std::endl << std::endl;

			winnerContent << "The following troops survived:" << std::endl;

			for (int i = 0; i < unitsLeft.size(); ++i)
			{
				winnerContent << unitsLeft[i].count << " of ID: " << unitsLeft[i].unitId << std::endl;
			}

			if (loserUnitsLeft.size() > 0)
			{
				loserContent << "The following troops survived:" << std::endl;

				for (int i = 0; i < loserUnitsLeft.size(); ++i)
				{
					loserContent << loserUnitsLeft[i].count << " of ID: " << loserUnitsLeft[i].unitId << std::endl;
				}

				loserContent << std::endl << "They will begin travelling home immediately" << std::endl << std::endl;
			}

			if (attackWin)
			{
				winnerContent << std::endl << "They will begin travelling home immediately" << std::endl << std::endl;


				if (defenderCity)
				{
					int stolenWood = defendingPlayer->wood / 2;
					int stolenFood = defendingPlayer->food / 2;
					int stolenMetal = defendingPlayer->metal / 2;
					int stolenOrder = defendingPlayer->order / 2;

					reportContent << "The following resources were looted:" << std::endl <<
						"Food: " << stolenFood << std::endl <<
						"Wood: " << stolenWood << std::endl <<
						"Metal: " << stolenMetal << std::endl <<
						"Order: " << stolenOrder << std::endl << std::endl;

					defendingPlayer->wood -= stolenWood;
					defendingPlayer->food -= stolenFood;
					defendingPlayer->metal -= stolenMetal;
					defendingPlayer->order -= stolenOrder;

					attackingPlayer->wood += stolenWood;
					attackingPlayer->food += stolenFood;
					attackingPlayer->metal += stolenMetal;
					attackingPlayer->order += stolenOrder;
				}
			}
			else
			{
				Map::getTile(destination)->army = unitsLeft;
			}

			EventHub::Report* winReport = EventHub::Report::CreateReport(attackWin ? "You were victorious in your attack" : "You successfully defended your city!", reportContent.str() + winnerContent.str());
			EventHub::Report* lossReport = EventHub::Report::CreateReport(attackWin ? "You were raided" : "Your attack was crushed", reportContent.str() + loserContent.str());

			if (attackWin)
			{
				attackingPlayer->reports.push_back(winReport);
				defendingPlayer->reports.push_back(lossReport);
			}
			else
			{
				attackingPlayer->reports.push_back(lossReport);
				defendingPlayer->reports.push_back(winReport);
			}

			if (attackWin)
			{
				new ScheduledMoveArmyEvent(20, unitsLeft, origin, destination, owner);

				if (!defenderCity && loserUnitsLeft.size() > 0)
				{
					new ScheduledMoveArmyEvent(20, loserUnitsLeft, defendingPlayer->cityLocation, destination, defendingPlayer->userId);
				}
			}
			else
			{
				if (defenderCity)
				{
					for (int i = 0; i < unitsLeft.size(); ++i)
					{
						defendingPlayer->homeArmy[unitsLeft[i].unitId] = unitsLeft[i].count;
					}
				}
				else
				{
					targetTile->army = unitsLeft;
				}

				if (loserUnitsLeft.size() > 0)
				{
					new ScheduledMoveArmyEvent(20, loserUnitsLeft, attackingPlayer->cityLocation, destination, attackingPlayer->userId);

				}
			}

			delete this;
		}

		ScheduledUnitUpgradeEvent::ScheduledUnitUpgradeEvent(int secondsTotal, int unitId, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->unitId = unitId;
			this->type = UNITUPGRADE;
		}

		void ScheduledUnitUpgradeEvent::Complete()
		{
			ScheduledEvent::Complete();

			Player::User* user = Game::getUserById(owner);

			int unitType = unitId / 10;
			switch (unitType)
			{
				// Archer
			case 0:
			{
				user->archerLevel = unitId;
				break;
			}
			// Cavalry
			case 1:
			{
				user->cavalryLevel = unitId;
				break;
			}
			// Swordsman
			case 2:
			{
				user->swordLevel = unitId;
				break;
			}
			// Spearman
			case 3:
			{
				user->spearmanLevel = unitId;
				break;
			}
			}

			user->unitUpgrading = false;

		}
	}
}
