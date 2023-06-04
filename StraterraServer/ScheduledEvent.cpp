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
			if (type == MAPBUILDING)
			{
				//std::cout << std::to_string((long)this) << " had Tick called (ticks left: " << this->secondsLeft << ")" << std::endl;
				std::cout << "data on tick: " << this->owner << " | " << this->secondsTotal << " | " << this->secondsLeft << " | " << this->type << std::endl;
			}
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
			Game::getUserById(owner)->homeArmy[unitId]++;
			amount--;
			if (amount == 0)
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

				//std::cout << "Added " << amount << " " << unitId << " to army! (Total: " << std::to_string(Game::getUserById(owner)->homeArmy[unitId]) << ")" << std::endl;
				delete this;
			}
			else
			{
				secondsLeft = secondsTotal;
			}
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
			Definition::TownBuilding townBuilding = Definition::getTownBuildingDefinition(buildingId);

			ScheduledEvent::Complete();
			Player::User* user = Game::getUserById(owner);

			EventHub::Report* report = EventHub::Report::CreateReport("Town Building Complete", "Add description here");
			user->reports.push_back(report);

			user->cityBuildingSlots[buildingSlot] = buildingId;


			if (townBuilding.type == Definition::TOWNHALL)
			{
				user->cityUpgradeCap = townBuilding.level;
			}

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

			std::cout << "ScheduledMapBuildingEvent finished initializing, position: " << position << " id: " << buildingId << std::endl;
		}
		void ScheduledMapBuildingEvent::Complete()
		{
			
			try
			{
				std::cout << "ScheduledMapBuildingEvent complete" << std::endl;
				std::cout << "Tileposition: " << position << " Owner: " << owner << " Building: " << buildingId << std::endl;
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
					user->metalGeneration += metalProd;
				}
				if (mapBuilding.type == Definition::HOUSE)
				{
					// maybe add something that increase or decrease this amount sometime
					int populationIncrease = mapBuilding.baseProduction * 1;
					user->populationCap += populationIncrease;
				}

				tile->building = buildingId;

				EventHub::Report* report = EventHub::Report::CreateReport("Map Building Complete", "Add description here");
				user->reports.push_back(report);
			}
			catch (const std::exception& e)
			{
				std::cout << e.what() << std::endl;
			}

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

			Player::User* armyOwner = Game::getUserById(owner);

			EventHub::Report* report = EventHub::Report::CreateReport("Army has arrived at " + std::to_string(destination), "Add description here");
			armyOwner->reports.push_back(report);

			Map::Tile* destinationTile = Map::getTile(destination);
			Map::Tile* originTile = Map::getTile(origin);

			if (destinationTile->building == 0)
			{
				if (originTile->building == 1)
				{
					new ScheduledMoveArmyEvent(0, army, origin, destination, owner);
					delete this;
					return;
				}
				std::cout << "Something went wrong in scheduledevent" << std::endl;
				delete this;
				return;
			}

			if (destinationTile->building == 1)
			{
				if (destinationTile->owner == owner)
				{
					for (int i = 0; i < army.size(); ++i)
					{
						armyOwner->homeArmy[army[i].unitId] += army[i].count;
					}
					delete this;
					return;
				}
				else
				{
					std::cout << "Something went wrong in scheduledevent" << std::endl;
					delete this;
					return;
				}
			}

			if (destinationTile->owner == owner)
			{
				for (int i = 0; i < army.size(); ++i)
				{
					bool merged = false;
					for (int k = 0; k < destinationTile->army.size(); ++k)
					{
						if (destinationTile->army[k].unitId == army[i].unitId)
						{
							merged = true;
							destinationTile->army[k].count += army[i].count;
						}
					}
					if (!merged)
					{
						destinationTile->army.push_back(Game::Group{ army[i].count, army[i].unitId });
					}
				}
				delete this;
				return;
			}
			else
			{
				if (destinationTile->army.size() > 0)
				{
					new ScheduledMoveArmyEvent(20, army, origin, destination, owner);
					delete this;
					return;
				}
				else
				{
					new ScheduledMoveArmyEvent(20, army, origin, destination, owner);
					delete this;
					return;

					// Maybe just take over the tile here but right now there
					// is no way for a player to naturally take over a tile without
					// building on it, so this would be unexpected to the player

					/*
					for (int i = 0; i < army.size(); ++i)
					{
						destinationTile->army.push_back(army[i]);
					}
					destinationTile->owner = owner;
					*/
				}
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

			Player::User* attacker = Game::getUserById(owner);
			Player::User* defender = Game::getUserById(targetTile->owner);

			if (targetTile->building == 1)
			{

				EventHub::Report* defenceWarning = EventHub::Report::CreateReport("Incoming attack on your city!", "You are being attacked by " + attacker->name + ", the attack will arrive in " + std::to_string(secondsTotal) + " seconds.");

				defender->reports.push_back(defenceWarning);
			}
			else
			{
				EventHub::Report* defenceWarning = EventHub::Report::CreateReport("Incoming attack on your land!", "You are being attacked by " + attacker->name + ", the attack will arrive in " + std::to_string(secondsTotal) + " seconds at position: [" + std::to_string(destination) + "]");
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

			int defenderPopulation = 0;
			int attackerPopluation = 0;

			for (int i = 0; i < defender.size(); ++i)
			{
				defenderPopulation += defender[i].count;
			}

			for (int i = 0; i < army.size(); ++i)
			{
				attackerPopluation += army[i].count;
			}


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
				else
				{
					if (targetTile->building > 0)
					{
						Definition::MapBuilding building = Definition::getMapBuildingDefinition(targetTile->building);
						switch (building.type)
						{
						case Definition::CASTLE:
							break;
						case Definition::FARM:
						{
							int foodProd = building.baseProduction * targetTile->foodAmount;
							defendingPlayer->foodGeneration -= foodProd;
							break;
						}
						case Definition::HOUSE:
						{
							int population = building.baseProduction;
							defendingPlayer->populationCap -= population;
							break;
						}
						case Definition::MINE:
						{
							int metalProd = building.baseProduction * targetTile->metalAmount;
							defendingPlayer->metalGeneration -= metalProd;
							break;
						}
						case Definition::WOOD:
						{
							int woodProd = building.baseProduction * targetTile->woodAmount;
							defendingPlayer->woodGeneration -= woodProd;
							break;
						}
						}
						targetTile->building = 0;
						targetTile->owner = -1;
					}
				}
			}
			else
			{
				targetTile->army = unitsLeft;
			}

			EventHub::Report* winReport = EventHub::Report::CreateReport(attackWin ? "You were victorious in your attack" : "You successfully defended your city!", reportContent.str() + winnerContent.str());
			EventHub::Report* lossReport = EventHub::Report::CreateReport(attackWin ? "You were raided" : "Your attack was crushed", reportContent.str() + loserContent.str());


			int defenderPopulationLeft = 0;
			int attackerPopulationLeft = 0;



			if (attackWin)
			{
				for (int i = 0; i < unitsLeft.size(); ++i)
				{
					attackerPopulationLeft += unitsLeft[i].count;
				}
				for (int i = 0; i < loserUnitsLeft.size(); ++i)
				{
					defenderPopulationLeft += loserUnitsLeft[i].count;
				}
				attackingPlayer->reports.push_back(winReport);
				defendingPlayer->reports.push_back(lossReport);
			}
			else
			{
				for (int i = 0; i < unitsLeft.size(); ++i)
				{
					defenderPopulationLeft += unitsLeft[i].count;
				}
				for (int i = 0; i < loserUnitsLeft.size(); ++i)
				{
					attackerPopulationLeft += loserUnitsLeft[i].count;
				}
				attackingPlayer->reports.push_back(lossReport);
				defendingPlayer->reports.push_back(winReport);
			}

			attackingPlayer->population -= attackerPopluation - attackerPopulationLeft;
			defendingPlayer->population -= defenderPopulation - defenderPopulationLeft;


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
		ScheduledInternalUpgradeEvent::ScheduledInternalUpgradeEvent(int secondsTotal, int upgradeId, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->upgradeId = upgradeId;
			this->type = INTERALUPGRADE;
		}
		void ScheduledInternalUpgradeEvent::Complete()
		{
			Player::User* user = Game::getUserById(owner);
			Definition::InternalUpgrade* upgrade = Definition::getInternalUpgradeDefinition(upgradeId);

			switch (upgrade->type)
			{
			case Definition::STORAGEUPGRADE:
			{
				Definition::StorageUpgrade* storageUpgrade = (Definition::StorageUpgrade*)upgrade;
				int food = storageUpgrade->foodMax;
				int wood = storageUpgrade->woodMax;
				int metal = storageUpgrade->metalMax;
				int order = storageUpgrade->orderMax;
				if (food > -1) user->foodMax = food;
				if (wood > -1) user->woodMax = wood;
				if (metal > -1) user->metalMax = metal;
				if (order > -1) user->orderMax = order;
				break;
			}
			case Definition::SMITHYUPGRADE:
			{
				break;
			}
			}

			EventHub::Report* report = EventHub::Report::CreateReport("Upgrade (id: " + std::to_string(upgrade->id) + ") Complete", "Add description here");
			user->reports.push_back(report);

			delete this;
		}
	}
}
