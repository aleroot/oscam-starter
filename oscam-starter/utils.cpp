//
//  utils.cpp
//  oscam-starter
//
//  Created by Alessio Pollero on 30/04/21.
//

#include "utils.hpp"

#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <array>
#include <algorithm>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end()); //Trim new line
    return result;
}

std::string which(std::string executable) {
    return exec(("which " + executable).c_str());
}
