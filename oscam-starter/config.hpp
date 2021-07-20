//
//  config.hpp
//  oscam-starter
//
//  Created by Alessio Pollero on 19/06/21.
//

#ifndef config_hpp
#define config_hpp

#include <string>
#include <iostream>
#include <fstream>

const std::string OSCAM = "oscam";
static const std::string OSCAM_CONFIG_PATH = "/etc/oscam";
static const std::string OSCAM_CONFIG_FILE = "oscam.conf";
static const std::string OSCAM_CONFIG_KPOR = "httpport";
static const std::string OSCAM_CONFIG_KUSR = "httpuser";
static const std::string OSCAM_CONFIG_KPWD = "httppwd";
static const char* OSCAM_PORT_ENV_VAR_NAME = "OSCAM_PORT";
static const char* OSCAM_USER_ENV_VAR_NAME = "OSCAM_USER";
static const char* OSCAM_PASS_ENV_VAR_NAME = "OSCAM_PASS";
static const char* OSCAM_ALIVE_PAGE = "/status.html?hideidle=0";
static auto CHECK_MAX_RETRY  = 3;

class ConfigExtractor {
public:
    ConfigExtractor(std::string configF);
    ~ConfigExtractor();
    std::string extractValue(std::string key, std::string defaultVal);
private:
    std::ifstream configFile;
};



#endif /* config_hpp */
