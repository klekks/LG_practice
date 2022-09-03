#pragma once

#include <map>
#include <string>
#include <boost/asio.hpp>
#include "boost/property_tree/ptree.hpp"

struct HttpResponse
{
	int status = 0;
	std::map<std::string, std::string> headers;
	boost::property_tree::ptree json;
};

using boost::asio::ip::tcp;
using boost::asio::ip::address;
using boost::asio::io_service;
using boost::asio::buffer;

void HttpPostJsonRequest(std::map<std::string, std::string> params,
						 std::string                        host, 
						 std::string                        path,
						 HttpResponse*                      http_response);

void HTTPRequest(std::map<std::string, std::string> params,
				 std::string                        host,
				 std::string                        path,
				 tcp::socket					   *socket,
				 tcp::resolver::iterator           *endpoint_iterator);

void readResponseHeaders(HttpResponse* http_response, std::istream* response_stream);

void readResponseBody(boost::asio::streambuf* response,
					  tcp::socket* socket,
					  HttpResponse* http_response);