//
//  main.cpp
//  oscam-starter
//
//  Created by Alessio Pollero on 29/04/21.
//
#include "starter.hpp"
#include "watchdog.hpp"

/**
 * OSCam starter is a simple application to start OSCam and monitor if the application is running correctly
 * querying its web interface, in this way it can prevent the OSCam hanging and going unresponsive; by default
 * the web interace is queried every 3 minutes and if unresponsive for more than 9 minutes the app gets killed
 * and OSCam starter quit so that SystemD can restart it promptly. In case the started is built with SYSTEMD_NOTIFY
 * macro defined theh the SystemD watchdog will be notifed, see watchdog.hpp documentation for more information.
 *
 * NOTE: This starter is meant to be used with SystemD, at the moment classic SysV init.d is not supported.
 *       In order to enable full SystemD notification install libsystemd-dev(Debian) or systemd-devel(OpenSuse) 
 *       and compile with -DSYSTEMD_NOTIFY .
 */
int main(int argc, const char * argv[]) {
    signal(SIGALRM,(void (*)(int))kill_oscam);
    signal(SIGHUP,(void (*)(int))reload_oscam);
    std::vector < const char * > arguments;
    for(int i = 1; i < argc; i++)
        arguments.push_back( argv[i] );
    
    Watchdog watchdog(OScamPoint::parseFromConfig(arguments));
    if(watchdog.start()) {
        Starter starter;
        const int e_code = starter.start(arguments);
        watchdog.stop();
        std::cout << "OScam exited with code: " << e_code << "\n";
        return e_code;
    } else {
        return 256;
    }
}
