#ifndef PLAYER_H
#define PLAYER_H

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
	namespace Player
	{
		void getSelfPlayer(long long token, std::string* out, int* code);
		void login(std::string* out, int* code, std::string loginInfo);
		//long long createSessionToken();
		void createUser(std::string name, std::string loginInfo);
	}
}


#endif