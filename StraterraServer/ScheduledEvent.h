#pragma once
#ifndef SCHEDULED_H
#define SCHEDULED_H

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
	namespace ScheduledEvents
	{
		int* getTestP();
		enum EventType
		{
			GENERIC = 0,
			UNITPRODUCTION = 1,
			TOWNBUILDING = 2,
			MAPBUILDING = 3
		};
		class ScheduledEvent
		{
		public:
			int secondsLeft;
			int secondsTotal;
			EventType type;
			bool running;
			int owner;
			ScheduledEvent(int secondsTotal, int owner, bool runImmediately = true);
			void Run();
			void Tick();
			virtual void Complete();
			boost::signals2::connection subcription;
		protected:
			std::vector<ScheduledEvent*>* ownerEvents;
		private:

		};


		class ScheduledUnitProductionEvent : ScheduledEvent
		{
		public:
			int unitId;
			int amount;
			ScheduledUnitProductionEvent(int secondsTotal, int unitId, int amount, int owner, bool runImmediately = true);
			void Complete();
		};

		class ScheduledTownBuildingEvent : ScheduledEvent
		{
		public:
			int buildingId;
			int buildingSlot;
			ScheduledTownBuildingEvent(int secondsTotal, int buildingId, int buildingSlot, int owner, bool runImmediately = true);
			void Complete();
		};

		class ScheduledMapBuildingEvent : ScheduledEvent
		{
		public:
			int buildingId;
			int position;
			ScheduledMapBuildingEvent(int secondsTotal, int buildingId, int posisition, int owner, bool runImmediately = true);
			void Complete();
		};
	}
}

#endif