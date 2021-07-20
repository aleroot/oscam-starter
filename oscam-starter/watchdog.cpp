//
//  watchdog.cpp
//  oscam-starter
//
//  Created by Alessio Pollero on 30/04/21.
//

#include "watchdog.hpp"
#include "config.hpp"
#include "libs/httplib/httplib.h"
#include <signal.h>
#include <thread>

#ifdef SYSTEMD_NOTIFY
#include <systemd/sd-daemon.h>  // sd_notify
#endif

static const auto HTTP_GET_TIMEOUT = std::chrono::seconds(5);

OScamPoint OScamPoint::parseFromEnv() {
    OScamPoint result = OScamPoint();
    if(const char* env_p = std::getenv(OSCAM_PORT_ENV_VAR_NAME))
        result.port = atoi(env_p);
    
    if(const char* env_u = std::getenv(OSCAM_USER_ENV_VAR_NAME))
        result.username = env_u;
    
    if(const char* env_p = std::getenv(OSCAM_PASS_ENV_VAR_NAME))
        result.password = env_p;
    
    return result;
}

OScamPoint OScamPoint::parseFromConfig(const std::vector < const char * >& arguments) {
    std::string def_path = OSCAM_CONFIG_PATH;
    for(int i = 0; i < arguments.size(); i++) {
        const char * arg = arguments.at(i);
        char c;
        int counter = 0;
        while ((c = (*arg++))) {
            if (++counter == 2 && (c == 'c' && ((*(arg - 2)) == '-'))) {
                def_path = arguments.at(i+1);
                goto load_file;
            }
        }
    }
    
    load_file:
    ConfigExtractor config(def_path + "/" + OSCAM_CONFIG_FILE);
    OScamPoint result = OScamPoint();
    result.port = std::stoi(config.extractValue(OSCAM_CONFIG_KPOR, std::to_string(DEFAULT_OSCAM_PORT)));
    result.username = config.extractValue(OSCAM_CONFIG_KUSR, "");
    result.password = config.extractValue(OSCAM_CONFIG_KPWD, "");
    return result;
}

bool OScamPoint::hasAuthentication() {
    return !this->username.empty() || !this->password.empty();
}

Watchdog::Watchdog() : check_interval(getInterval()) {}

Watchdog::Watchdog(std::chrono::milliseconds interval) : check_interval(interval) {}

Watchdog::Watchdog(OScamPoint oscam) : check_interval(getInterval()), oscamAddress(oscam) {}

bool Watchdog::start() {
    std::thread watcher(&Watchdog::watch, this);
    this->is_stopped = false;
    this->stop_req = false;
    watcher.detach();
#ifdef SYSTEMD_NOTIFY
    sd_notify(0, "READY=1");
#endif
    return !this->is_stopped;
}

bool Watchdog::stop() {
#ifdef SYSTEMD_NOTIFY
    sd_notify(0, "STOPPING=1");
#endif
    stop_req = true;
    while(!is_stopped) { }
    return is_stopped;
}

void Watchdog::watch() {
    int failure_count = 0;
    long long prev_exec_cost = 0;
    httplib::Client cli(oscamAddress.hostname, oscamAddress.port);
    cli.set_connection_timeout(HTTP_GET_TIMEOUT);
    cli.set_read_timeout(HTTP_GET_TIMEOUT);
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    if(oscamAddress.hasAuthentication())
        cli.set_digest_auth(oscamAddress.username.c_str(), oscamAddress.password.c_str());
#endif
    
    while(!stop_req) {
        interruptibleWait(check_interval.count() - prev_exec_cost); //wait
        auto started = std::chrono::high_resolution_clock::now();
        if(!stop_req) {
            if (auto res = cli.Get(OSCAM_ALIVE_PAGE)) {
                const int scode = res->status;
                if (scode >= 200 && scode < 500) {
                #ifdef SYSTEMD_NOTIFY
                    sd_notify(0, "WATCHDOG=1");
                #endif
                    failure_count = 0;
                } else
                    failure_count++;
                
            } else
                failure_count++;
            
            if(failure_count >= CHECK_MAX_RETRY) {
                alarm(30);
                break;
            }
        }
        auto done = std::chrono::high_resolution_clock::now();
        prev_exec_cost = std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count();
    }
    is_stopped = true;
}

void Watchdog::interruptibleWait(long long milliseconds) {
    static const int max_cpu_wait = 10; //milliseconds the CPU sleep
    const long long rounds = milliseconds / max_cpu_wait;
    for(int i = 0; !stop_req && i <= rounds; i++)
        std::this_thread::sleep_for (std::chrono::milliseconds(max_cpu_wait));
}

std::chrono::milliseconds Watchdog::getInterval() {
    const char* watchdog_usec = std::getenv("WATCHDOG_USEC");
    if (watchdog_usec) {
        const unsigned int seconds = (atoi(watchdog_usec) / (1000 * 1000)) / 2;
        return std::chrono::seconds(seconds);
    }
    return std::chrono::minutes(3);
}
