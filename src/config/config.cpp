/*
 * Copyright (C) 2021 LG Electronics Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BOOST_BIND_GLOBAL_PLACEHOLDERS // can be removed once this is fixed:
                                       // https://github.com/boostorg/property_tree/pull/50
#include <boost/property_tree/json_parser.hpp>
#undef BOOST_BIND_GLOBAL_PLACEHOLDERS
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <regex>

#include "config/config.h"

static inline std::string remove_undesired_chars(const std::string &s) {
  std::regex html_regex(R"([^ A-Za-z0-9:~._/=%\\()\r\n\t\?#-]?)");
  return std::regex_replace(s, html_regex, "");
}

static Config JsonToConfig_(std::istream &js, Config &config) {
  try {
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(js, pt);

    for (auto const &pair : pt) {
      auto const &key = pair.first;
      auto const &val = pair.second;
      if (key == "ws_port") {
        config.set_http_port(val.get_value<int>());
      } else if (key == "http_port") {
        config.set_http_port(val.get_value<int>());
      } else if (key == "log_severity") {
        config.set_log_severity(val.get_value<int>());
      } else if (key == "interface_name") {
        config.set_interface_name(
            remove_undesired_chars(val.get_value<std::string>()));
      } else if (key == "syslog_proto") {
        config.set_syslog_proto(
            remove_undesired_chars(val.get_value<std::string>()));
      } else if (key == "syslog_server") {
        config.set_syslog_server(
            remove_undesired_chars(val.get_value<std::string>()));
      } else if (key == "ip_addr") {
        /* ignored */
      } else {
        std::cerr << "Warning: unkown configuration option " << key
                  << std::endl;
      }
    }
  } catch (boost::property_tree::json_parser::json_parser_error &je) {
    throw std::runtime_error("error parsing JSON at line " +
                             std::to_string(je.line()) + " :" + je.message());
  } catch (...) {
    throw std::runtime_error("failed to convert a number");
  }
  return config;
}

static Config JsonToConfig(std::istream &js) {
  Config config;
  return JsonToConfig_(js, config);
}

std::shared_ptr<Config> Config::Parse(const std::string &filename) {
  Config config;

  std::ifstream jsonstream(filename);
  if (!jsonstream) {
    std::cerr << "Fatal: error opening config file " << filename << std::endl;
    return nullptr;
  }

  try {
    config = JsonToConfig(jsonstream);
  } catch (std::exception const &e) {
    std::cerr << "Configuration file " << filename << " : " << e.what()
              << std::endl;
    return nullptr;
  }

  if (config.log_severity_ < 0 || config.log_severity_ > 5)
    config.log_severity_ = 2;

  return std::make_shared<Config>(config);
}
