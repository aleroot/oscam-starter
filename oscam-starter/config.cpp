//
//  config.cpp
//  oscam-starter
//
//  Created by Alessio Pollero on 19/06/21.
//

#include "config.hpp"
#include <vector>
#include <algorithm>

inline std::string trim(const std::string &s)
{
   auto  wsfront=std::find_if_not(s.begin(),s.end(),[](int c){return std::isspace(c);});
   return std::string(wsfront,std::find_if_not(s.rbegin(),std::string::const_reverse_iterator(wsfront),[](int c){return std::isspace(c);}).base());
}

void tokenize(std::string const &str, const char delim,
            std::vector<std::string> &out)
{
    size_t start;
    size_t end = 0;
 
    while ((start = str.find_first_not_of(delim, end)) != std::string::npos)
    {
        end = str.find(delim, start);
        out.push_back(trim(str.substr(start, end - start)));
    }
}

ConfigExtractor::ConfigExtractor(std::string configF) {
    configFile.open(configF);
}

ConfigExtractor::~ConfigExtractor() {
    if(configFile.is_open())
        configFile.close();
}

std::string ConfigExtractor::extractValue(std::string key, std::string defaultVal) {
    if (configFile.is_open()) {
        std::string line;
        while (std::getline(configFile, line)) {
            std::vector<std::string> out;
            tokenize(line, '=', out);
            if(out.size() > 1) {
                if(key == out.at(0)) {
                    return out.at(1);
                }
            }
        }
    }
    return  defaultVal;
}
