#pragma once
#include <string>

/*
* Config structure template. 
*/
typedef struct {
	std::string realm;
	std::string realm_public_key;
	std::string auth_server_url;
	std::string resource; // resource == client_id
	std::string secret;
} KeycloakConf;


