#pragma once

#include <string>

#include "keycloakconf.hpp"
#include "request.hpp"

using std::string;


class AuthorizationController
{
public:
    AuthorizationController(KeycloakConf config);

    string getAccessToken();

private:

    KeycloakConf configuration;
    string HOST;
    string PATH;

    string access_token;
    time_t expires_in;
    time_t refresh_expires_in;
    string refresh_token;
    string token_type;

    void RefreshAccessToken();

    void RequestAccessToken();

    void parseResponse(HttpResponse response);
};