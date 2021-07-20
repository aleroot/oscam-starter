//
//  starter.hpp
//  oscam-starter
//
//  Created by Alessio Pollero on 29/04/21.
//

#ifndef starter_hpp
#define starter_hpp

#include <iostream>
#include <string>
#include <vector>
#include <signal.h>

void kill_oscam(int sig);
void reload_oscam(int sig);

class Starter {
private:
    std::string path;
public:
    Starter();
    Starter(std::string path);
    int start();
    int start(std::vector < const char * > args);
};

#endif /* starter_hpp */
