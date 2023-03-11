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

namespace Straterra
{
	namespace EventHub
	{
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
			//std::cout << "TESTP: " << *(ScheduledEvents::getTestP()) << std::endl;
			onTick();
		}
	}
}