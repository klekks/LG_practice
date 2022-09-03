
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <string>

#include "request.hpp"

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

void HttpPostJsonRequest(std::map<std::string, std::string> params,
                         std::string                        host,
                         std::string                        path,
                         HttpResponse* http_response)
{
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host.c_str(), "http");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);

    HTTPRequest(params, host, path, &socket, &endpoint_iterator);

    boost::asio::streambuf response;
    boost::asio::read_until(socket, response, "\r\n"); // until http head end

    std::istream response_stream(&response);
    std::string http_version;
    response_stream >> http_version;
    response_stream >> http_response->status;
    std::string status_message;
    std::getline(response_stream, status_message);

    boost::asio::read_until(socket, response, "\r\n\r\n");  //  until end of headers

    readResponseHeaders(http_response, &response_stream);
    readResponseBody(&response, &socket, http_response);

}

void HTTPRequest(std::map<std::string, std::string> params,
                 std::string                        host,
                 std::string                        path,
                 tcp::socket                       *socket,
                 tcp::resolver::iterator           *endpoint_iterator)
{
    
    boost::asio::connect(*socket, *endpoint_iterator);

    boost::asio::streambuf request;
    std::ostream request_stream(&request);
    request_stream << "POST " << path << " HTTP/1.0\r\n";
    request_stream << "Host: " << host << "\r\n";
    request_stream << "Accept: */*\r\n";
    request_stream << "Connection: close\r\n";
    request_stream << "Content-Type: application/x-www-form-urlencoded\r\n";

    std::string query_str = "";
    for (auto iter = params.begin(); iter != params.end();)
    {
        query_str += iter->first + "=" + iter->second;
        if (++iter == params.end()) break;
        query_str += "&";
    }
    request_stream << "Content-Length: " << query_str.size() << "\r\n\r\n";
    request_stream << query_str;

    boost::asio::write(*socket, request);
}

void readResponseHeaders(HttpResponse* http_response, std::istream *response_stream)
{
    std::string header;

    while (std::getline(*response_stream, header) && header != "\r")
    {
        size_t headerNameLength = header.find(":");
        http_response->headers.insert(
                                        std::pair<std::string, std::string>
                                            (
                                                header.substr(0, headerNameLength),                // header name
                                                header.substr(headerNameLength + 1, header.size()) // header value
                                            )
                                    );
    }
}

void readResponseBody(boost::asio::streambuf *response, 
                      tcp::socket            *socket,
                      HttpResponse           *http_response)
{
    boost::system::error_code error;
    while (boost::asio::read(*socket, *response,
        boost::asio::transfer_at_least(1), error));
    if (error != boost::asio::error::eof)
        throw boost::system::system_error(error);

    std::stringstream response_body;
    response_body << response;
    read_json(response_body, http_response->json);
}