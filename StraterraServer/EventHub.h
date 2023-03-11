#ifndef EVENT_H
#define EVENT_H

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio.hpp>
#include <boost/signals2/signal.hpp>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory>
#include <string>

namespace Straterra
{
	namespace EventHub
	{
		boost::signals2::connection subcribeOnTick(const boost::signals2::slot<void(), boost::function<void()>>& f);
		void unsubcribeOnTick(boost::signals2::connection* c);
		void fireOnTick();
	}
}

#endif