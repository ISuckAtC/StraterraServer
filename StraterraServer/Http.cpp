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
				// target location requested by client
				std::string target = request_.target().to_string();
				std::cout << "Target: \"" << target << "\"" << std::endl;

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


				if (method == "a")
				{
					// set content type (http is binary)
					response_.set(http::field::content_type, "http");

					// write content to stream
					beast::ostream(response_.body())
						<< _code++;
				}
				else if (method == "getSelfPlayer")
				{
					if (optionCount != 2)
					{
						// option count mismatch
					}
					std::string out = "";
					int code = 0;
					char token[sizeof(long long)];
					strcpy_s(token, options[0].c_str());
					Straterra::Player::getSelfPlayer(Straterra::Game::getTokenLong(token), &out, &code);
					response_.set(http::field::content_type, "json");

					beast::ostream(response_.body())
						<< out;
				}
				else if (method == "createPlayer")
				{
					if (optionCount != 2)
					{
						// option count mismatch
					}
					std::string name = options[0];
					std::string login = options[1];
					Straterra::Player::createUser(name, login);
					response_.set(http::field::content_type, "text");
					beast::ostream(response_.body())
						<< "Created user: \"" << name << "\" with login \"" << login << "\"";
				}
				else if (method == "login")
				{
					if (optionCount != 1)
					{
						// option count mismatch
					}
					std::string login = options[0];
					std::string out = "";
					int code = 0;
					
					Straterra::Player::login(&out, &code, login);
					response_.set(http::field::content_type, "http");
					beast::ostream(response_.body())
						<< out;
					
				}
				else
				{
					response_.set(http::field::content_type, "http");
					beast::ostream(response_.body())
						<< 0x01;
				}
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
			std::cout << "Waiting for connection..." << std::endl;
			acceptor.async_accept(socket,
				[&](beast::error_code ec)
				{
					if (!ec)
					{

						std::make_shared<http_connection>(std::move(socket))->start();
						http_server(acceptor, socket);
					}
				});
		}
	}
}

