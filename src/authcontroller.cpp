#include "authcontroller.hpp"
#include <map>
#include <regex>
#include <iostream>

using std::map;
using std::string;
using boost::system::error_code;
using boost::system::system_error;

AuthorizationController::AuthorizationController(KeycloakConf config)
{
    this->configuration = config;
    /*
    * url = https://sub.domain.com/some/path
    * url => domain = "sub.domain.com"
    *        path   = "/some/path"
    */
    string url = config.auth_server_url;
    url = std::regex_replace(url, std::regex("(https?)?:\/\/"), "");
    size_t path_index = url.find('/');
    if (path_index == string::npos)
    {
        this->HOST = url;
        this->PATH = "/";
    }
    else
    {
        this->HOST = url.substr(0, path_index);
        this->PATH = url.substr(path_index);
    }
}

string AuthorizationController::getAccessToken()
{
    if (this->access_token == "")
        this->RequestAccessToken();
    else if (this->expires_in < time(NULL))
        if (this->refresh_expires_in < time(NULL))
            this->RequestAccessToken();
        else
            this->RefreshAccessToken();
    if (this->expires_in < time(NULL))
        throw ("BAD TOKEN\n");
    return this->access_token;
}

void AuthorizationController::RefreshAccessToken()
{
    map<string, string> requestParams;
    requestParams.insert(std::pair<string, string>("grant_type", "refresh_token"));
    requestParams.insert(std::pair<string, string>("refresh_token", this->refresh_token));

    HttpResponse jsonResponse;
    HttpPostJsonRequest(requestParams, this->HOST, this->PATH, &jsonResponse);

    if (jsonResponse.status != 200)
    {
        std::cout << "Access  request status " << jsonResponse.status << std::endl;
        return;
    }

    this->parseResponse(jsonResponse);
}

void AuthorizationController::RequestAccessToken()
{
    map<string, string> requestParams;
    requestParams.insert(std::pair<string, string>("grant_type", "client_credentials"));
    requestParams.insert(std::pair<string, string>("client_id", configuration.resource));
    requestParams.insert(std::pair<string, string>("client_secret", configuration.secret));

    HttpResponse jsonResponse;
    HttpPostJsonRequest(requestParams, this->HOST, this->PATH, &jsonResponse);

    if (jsonResponse.status != 200)
    {
        std::cout << "Access  request status " << jsonResponse.status << std::endl;
        throw("Invalid response status");
    }

    this->parseResponse(jsonResponse);
}

void AuthorizationController::parseResponse(HttpResponse response)
{
    this->access_token = response.json.get <string>("access_token");
    this->refresh_token = response.json.get <string>("refresh_token");
    this->expires_in = atoi(
        response.json.get <string>("expires_in").c_str()
    )
        + time(NULL);
    this->refresh_expires_in = atoi(
        response.json.get <string>("refresh_expires_in"
            ).c_str())
        + time(NULL);
    this->token_type = response.json.get <string>("token_type");
}
