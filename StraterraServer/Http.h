#pragma once

#ifndef HTTP_H
#define HTTP_H

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

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace Straterra
{
	namespace Http
	{
		static int _code;
		

		class http_connection : public std::enable_shared_from_this<http_connection>
		{
		public:
			http_connection(tcp::socket socket);

			void start();
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
			net::steady_timer deadline_;

			// underlying read operation from client
			void read();

			void handle_request();

			void create_response();

			// async send response
			void write_response();

			void check_deadline();
		};

		void http_server(tcp::acceptor& acceptor, tcp::socket& socket);
	}
}
#endif