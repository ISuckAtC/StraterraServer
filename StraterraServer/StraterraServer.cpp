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
#include <fstream>
#include "Http.h"
#include "Definition.h"
#include "Game.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
using namespace Straterra;



int main(int argc, char** argv)
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
	std::cout << "Argument list:" << std::endl;
	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
	}
	std::cout << std::endl;
	
	if (argc != 2)
	{
		std::cout << "Argument count invalid, excepted 2, is " << std::to_string(argc) << std::endl;
		return 1;
	}

	int lines = 0;
	std::ifstream data{ argv[1] };
	std::string line;
	while (std::getline(data, line))
	{
		int index = line.find_first_of(';');
		Game::User* u = new Game::User();
		u->userId = std::stoi(line.substr(0, index));

		line = line.substr(index + 1);
		index = line.find_first_of(';');
		u->login = line.substr(0, index);

		line = line.substr(index + 1);
		index = line.find_first_of(';');
		u->name = line.substr(0, index);

		line = line.substr(index + 1);
		index = line.find_first_of(';');
		u->cityLocation = std::stoi(line.substr(0, index));

		line = line.substr(index + 1);
		index = line.find_first_of(';');
		u->color = std::stoi(line.substr(0, index));

		line = line.substr(index + 1);
		index = line.find_first_of(';');
		u->allianceId = std::stoi(line.substr(0, index));

		for (int i = 0; i < 8; ++i)
		{
			line = line.substr(index + 1);
			index = line.find_first_of(';');
			u->cityBuildingSlots[i] = std::stoi(line.substr(0, index));
		}


		addUser(u);
	}
	

	try
	{
		Straterra::Game::start(std::stoi(argv[0]), 60000);

		auto const address = net::ip::make_address_v4("0.0.0.0");
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

