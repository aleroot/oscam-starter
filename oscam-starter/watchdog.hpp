//
//  watchdog.hpp
//  oscam-starter
//
//  Created by Alessio Pollero on 30/04/21.
//

#ifndef watchdog_hpp
#define watchdog_hpp

#include <atomic>
#include <chrono>
#include <string>
#include <vector>

static const auto DEFAULT_OSCAM_HOST  = "localhost";
static const auto DEFAULT_OSCAM_PORT  = 8888;

struct OScamPoint {
private:
    std::string hostname;
    std::string username;
    std::string password;
    int port;
    static std::vector< const char * > DEFAULT_VECTOR;
public:
    static OScamPoint parseFromEnv();
    static OScamPoint parseFromConfig(const std::vector < const char * >& arguments = DEFAULT_VECTOR);
    OScamPoint() : hostname(DEFAULT_OSCAM_HOST), port(DEFAULT_OSCAM_PORT) {}
    OScamPoint(std::string h, int p) : hostname(h), port(p) {}
    bool hasAuthentication();
    friend class Watchdog;
};

/** Watchdog integrated with Systemd daemon watchdog system(to enable integration, set macro; SYSTEMD_NOTIFY)
 *  in case Systemd doesn't restart the service ot Systemd integraton is not enabled the OSCam process is killed after 3
 *  times is not found responding to its HTTP listening port.
 *  The default check interval is 3 minutes(see  getInterval for more information)so after the end of each interval the OSCam service is checked and a
 *  a signal WATCHDOG=1 is sent to SystemD, after 3 consecutive failures the process is eventually killed.
 *  In order to enable Systemd watchdog for the unit file, the following parameters must be set in [Service] section:
 *      - WatchdogSec=5min 20s
 *      - Restart=on-failure
 */
class Watchdog {
private:
    std::chrono::milliseconds check_interval;
    std::atomic<bool> stop_req;
    bool is_stopped = false;
    OScamPoint oscamAddress;
    void watch();
    void interruptibleWait(long long milliseconds);
    /**
     * Retrieve the number of seconds to wait for between an helath check and another
     * if the SystemD watchdog is enabled the interval is retrieved from WATCHDOG_USEC env variable
     * and halved as requested by SystemD documentation.
     * If this program is not started with SystemD watchdog enable then a default
     * interval time of three minutes is set.
     */
    static std::chrono::milliseconds getInterval();
public:
    Watchdog();
    Watchdog(std::chrono::milliseconds interval);
    Watchdog(OScamPoint oscam);
    bool start();
    bool stop();
};

#endif /* watchdog_hpp */
