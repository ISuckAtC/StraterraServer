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
#include <boost/signals2/signal.hpp>

#include "Game.h"
#include "ScheduledEvent.h"
#include "EventHub.h"

namespace Straterra
{
	namespace EventHub
	{
		Report* Report::CreateReport(std::string title, std::string content)
		{
			Report* report = new Report();
			report->title = title;
			report->content = content;
			report->timeCreated = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			report->viewed = false;

			return report;
		}

		boost::signals2::signal<void()> onTick;
		boost::signals2::connection subcribeOnTick(const boost::signals2::slot<void(),boost::function<void()>> &f)
		{
			return onTick.connect(f);
		}
		void unsubcribeOnTick(boost::signals2::connection* c)
		{
			c->disconnect();
		}
		void fireOnTick()
		{
			try{
			//std::cout << "TESTP: " << *(ScheduledEvents::getTestP()) << std::endl;
			onTick();
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
	}
}