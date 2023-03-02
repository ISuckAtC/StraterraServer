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
#include "Definition.h"
#include "Game.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
//using namespace Straterra;



int main()
{
	Straterra::Definition::DefineUnits();
	/*
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
	*/
	
	

	try
	{
		Straterra::Game::start(1000, 60000);

		auto const address = net::ip::make_address_v4("18.216.109.151");
		unsigned short port = 80;

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

