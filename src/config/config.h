#pragma once

#include <cstdint>
#include <memory>
#include <string>

class Config {
public:
  /* build config from json file */
  static std::shared_ptr<Config> Parse(const std::string &filename);

  /* attributes retrieved from config json */
  uint16_t get_http_port() const { return http_port_; };
  int get_log_severity() const { return log_severity_; };
  const std::string &get_syslog_proto() const { return syslog_proto_; };
  const std::string &get_syslog_server() const { return syslog_server_; };
  const std::string &get_interface_name() const { return interface_name_; };


  void set_http_port(uint16_t http_port) { http_port_ = http_port; };
  void set_log_severity(int log_severity) { log_severity_ = log_severity; };
  void set_syslog_proto(const std::string &syslog_proto) {
    syslog_proto_ = syslog_proto;
  };
  void set_syslog_server(const std::string &syslog_server) {
    syslog_server_ = syslog_server;
  };
  void set_interface_name(const std::string &interface_name) {
    interface_name_ = interface_name;
  };

private:
  /* from json */
  int log_severity_{2};
  std::string syslog_proto_{""};
  std::string syslog_server_{""};
  std::string interface_name_{"eth0"};

  uint16_t http_port_{8000};
};
