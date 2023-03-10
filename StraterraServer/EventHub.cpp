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

namespace Straterra
{
	namespace EventHub
	{
		boost::signals2::signal<void()> onTick;
		void subcribeOnTick(const boost::signals2::slot<void(),boost::function<void()>> &f)
		{
			onTick.connect(f);
		}
		void unsubcribeOnTick(void* f)
		{
			onTick.disconnect(f);
		}
		void fireOnTick()
		{
			onTick();
		}
	}
}