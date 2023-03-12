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

#include "Player.h"
#include "Game.h"
#include "UserMethods.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>


namespace Straterra
{
	namespace Http
	{
		static int _code = 0;

		class http_connection : public std::enable_shared_from_this<http_connection>
		{
		public:
			http_connection(tcp::socket socket) : socket_(std::move(socket)) 
			{

			};

			void start()
			{
				read();
				check_deadline();
			}
		private:

			// the underlying socket to read and write on network
			tcp::socket socket_;

			// the buffer to contain messages
			beast::flat_buffer buffer_{ 1024 };

			// the request message recieved from the client
			http::request<http::dynamic_body> request_;

			// the response message sent back to the client
			http::response<http::dynamic_body> response_;

			// the amount of time to wait before timing out
			net::steady_timer deadline_{
				socket_.get_executor(), std::chrono::seconds(20) };

			// underlying read operation from client
			void read()
			{
				// pointer to this object
				auto self = shared_from_this();

				http::async_read(
					socket_,
					buffer_,
					request_,
					[self](
						beast::error_code ec,
						std::size_t bytes_recieved)
					{
						boost::ignore_unused(bytes_recieved);
						if (!ec) self->handle_request();
					});
			}

			void handle_request()
			{
				// match the version of the request
				response_.version(request_.version());

				// handle and drop
				response_.keep_alive(false);

				// what http method is used
				switch (request_.method())
				{
					// GET
				case http::verb::get:
					// response code
					response_.result(http::status::ok);

					// seems to be setting header
					response_.set(http::field::server, "S_beast");


					create_response();
					break;
				}

				write_response();
			}

			void create_response()
			{
				std::string out = "";
				int code = 0;
				try
				{
					// target location requested by client
					std::string target{ request_.target() };
					//std::cout << "Target: \"" << target << "\"" << std::endl;

					int optionCount = 0;
					std::string options[8];

					int queryIndex = target.find_first_of('?', 0);
					std::string method;
					if (queryIndex != std::string::npos)
					{
						method = target.substr(1, queryIndex - 1);
						int currentIndex = queryIndex + 1;

						int andIndex = target.find_first_of('&', currentIndex);

						while (andIndex != std::string::npos)
						{
							options[optionCount] = target.substr(currentIndex, andIndex - currentIndex);
							optionCount++;
							currentIndex = andIndex + 1;
							andIndex = target.find_first_of('&', currentIndex);
						}

						options[optionCount] = target.substr(currentIndex, target.find_first_of('\0'));
						optionCount++;
					}
					else
					{
						method = target.substr(1);
					}

					std::cout << "Method: \"" << method << "\"" << std::endl;

					//for (int i = 0; i < optionCount; ++i)
					//{
					//	std::cout << options[i] << std::endl;
					//}

					response_.set(http::field::content_type, "json");
					
					long long token = 0;
					if (method != "login" && method != "createPlayer")
					{
						try
						{
							token = Straterra::Game::getTokenLong(options[0]);
						}
						catch (std::exception const& e)
						{
							std::cerr << e.what() << std::endl;
						}
					}
					if (method == "getResources")
						UserMethods::getResources(token, std::stoi(options[1]), &out, &code);
					
					else if (method == "getSelfUser")
						UserMethods::getSelfPlayer(token, &out, &code);
					
					else if (method == "createPlayer")
						UserMethods::createUser(options[0], options[1]);
					
					else if (method == "login")
						UserMethods::login(&out, &code, options[0]);
					
					else if (method == "getUsers")
						UserMethods::getPlayers(token, &out, &code);
					
					else if (method == "getUser")
						UserMethods::getUser(token, std::stoi(options[1]), &out, &code);
					
					else if (method == "createTownBuilding")
						UserMethods::createTownBuilding(token, std::stoi(options[1]), std::stoi(options[2]), &out, &code);
					
					else if (method == "getScheduledEvents")
						UserMethods::getScheduledEvents(token, &out, &code);
					
					else if (method == "createUnits")
						UserMethods::createUnits(token, std::stoi(options[1]), std::stoi(options[2]), &out, &code);
					
					else if (method == "createMapBuilding")
						UserMethods::createMapBuilding(token, std::stoi(options[1]), std::stoi(options[2]), &out, &code);
					
					else if (method == "getHomeUnits")
						UserMethods::getHomeUnits(token, &out, &code);
					
					else
						out = "unknown method";
				}
				catch (std::exception& e)
				{
					std::cout << e.what() << std::endl;
					out = "error";
				}
				beast::ostream(response_.body()) << out;
			}

			// async send response
			void write_response()
			{
				// pointer to self
				auto self = shared_from_this();

				// set the amount of bytes being sent
				response_.content_length(response_.body().size());

				// write response to client
				http::async_write(
					socket_,
					response_,
					[self](beast::error_code ec, std::size_t bytes_sent)
					{
						// shut the socket down after use and cancel deadline
						self->socket_.shutdown(tcp::socket::shutdown_send, ec);
						self->deadline_.cancel();
					}
				);
			}

			void check_deadline()
			{
				auto self = shared_from_this();

				// async_wait just waits equal to whats passed to the timer
				deadline_.async_wait(
					[self](beast::error_code ec)
					{
						if (!ec)
						{
							// close connection if deadline is passed
							self->socket_.close(ec);
						}
					}
				);
			}
		};

		void http_server(tcp::acceptor& acceptor, tcp::socket& socket)
		{
			try
			{
				//std::cout << "Waiting for connection..." << std::endl;
				acceptor.async_accept(socket,
					[&](beast::error_code ec)
					{


						if (!ec)
						{
							std::make_shared<http_connection>(std::move(socket))->start();
							http_server(acceptor, socket);

						}
						std::cout << "EC set, exiting listener loop: " << ec.message() << std::endl;
					});
			}
			catch (std::exception const& e)
			{
				std::cout << "ERROR: " << e.what() << std::endl;
			}
		}
	}
}

