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
			std::cout << "CONSTRUCTOR VALUE: " << (long)(&(this->secondsLeft)) << std::endl;
			this->owner = owner;
			this->ownerEvents = &(Game::getUserById(owner)->activeEvents);
			this->type = GENERIC;
			if (runImmediately)
			{
				std::cout << "Creating scheduled event" << std::endl;
				auto a = boost::bind(&ScheduledEvent::Tick, this);
				EventHub::subcribeOnTick(a);
				this->running = true;
			}
			else
			{
				this->running = false;
			}
			this->ownerEvents->insert(this->ownerEvents->begin(), this);
			std::cout << "CONSTRUCTOR DONE" << std::endl;
		}

		void ScheduledEvent::Run()
		{
			running = true;
			subcription = EventHub::subcribeOnTick(boost::bind(&ScheduledEvent::Tick, this));
		}

		void ScheduledEvent::Tick()
		{
			std::cout << std::to_string((long)this) << " had Tick called (ticks left: " << secondsLeft << ")" << std::endl;
			if (secondsLeft-- == 0) Complete();
		}

		void ScheduledEvent::Complete()
		{
			// unsubcribe to tick event here
			// 
			// find iterator for own position and remove it from the vector
			EventHub::unsubcribeOnTick(&subcription);
			ownerEvents->erase(std::find(ownerEvents->begin(), ownerEvents->end(), this));
		}

		ScheduledUnitProductionEvent::ScheduledUnitProductionEvent(int secondsTotal, int unitId, int amount, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->unitId = unitId;
			this->amount = amount;
			this->type = UNITPRODUCTION;
			std::cout << "UNIT POINTER: " << (long)this << "| UNITPRODTIME: " << *testP << std::endl;
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
			std::cout << "Added " << amount << " " << unitId << " to army! (Total: " << Game::getUserById(owner)->homeArmy[unitId] << ")" << std::endl;
		}
		ScheduledTownBuildingEvent::ScheduledTownBuildingEvent(int secondsTotal, int buildingId, int buildingSlot, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->buildingId = buildingId;
			this->buildingSlot = buildingSlot;
			this->type = TOWNBUILDING;
		}
		void ScheduledTownBuildingEvent::Complete()
		{
			ScheduledEvent::Complete();
			Player::User* user = Game::getUserById(owner);
			user->cityBuildingSlots[buildingSlot] = buildingId;
		}

		ScheduledMapBuildingEvent::ScheduledMapBuildingEvent(int secondsTotal, int buildingId, int position, int owner, bool runImmediately) : ScheduledEvent(secondsTotal, owner, runImmediately)
		{
			this->buildingId = buildingId;
			this->position = position;
			this->type = MAPBUILDING;
		}
		void ScheduledMapBuildingEvent::Complete()
		{
			ScheduledEvent::Complete();
			Map::Tile* tile = Map::getTile(position);

			tile->building = buildingId;
		}
	}
}
