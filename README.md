#  OSCam Starter

OSCam starter is a simple application to start [OSCam](https://trac.streamboard.tv/oscam) and monitor if the application is running correctly periodically querying its web interface, in this way it can prevent the OSCam daemon going unresponsive.
This starter has been developed since [OSCam development team never wanted to integrate SystemD watchdog directly](https://trac.streamboard.tv/oscam/ticket/4038).
The starter is fully integrated with SystemD watchdog daemon so that it can delegate to SystemD the restart of the OSCam process in case of hung OSCam process . 
OSCam starter can also force a restart of the oscam process on its own in case the process will be still unresponsive after 3 consecutive failing periodic checks(this situation should never occur in case SystemD watchdog is enabled correctly). 

## Building

This repository contains an Xcode project that can simply open and built, a Makefile is also avaiable in order to build it on Linux or any other unix like platform, just run make from main repo directory:
``` 
make clean
make release
```
Binary files will be created in : `build/app` folder.
OpenSSL library(libssl-dev package on Debian) is required in order to enable the querying of a password protected instance of OSCam.
In order to enable full SystemD notification install `libsystemd-dev`(Debian) or `systemd-devel`(OpenSuse).

## Installing

To install oscam-starer simply copy the binary to `/usr/local/bin/` or any other in PATH location, or simply run `make install`. 

## SystemD integration

### Example of SystemD unit file 

**oscam.service**
``` 
[Unit]
Description=OScam
After=network.target
Requires=network.target
StartLimitInterval=0
[Service]
Type=notify
PIDFile=/run/oscam.pid
ExecStart=/usr/local/bin/oscam-starter -b -B /run/oscam.pid -c /etc/oscam
ExecStop=/usr/bin/rm /run/oscam.pid
ExecReload=/bin/kill -HUP $MAINPID
TimeoutStopSec=1
WatchdogSec=6min
Restart=on-failure
RestartSec=3
StartLimitInterval=0
[Install]
WantedBy=multi-user.target
```
The health check to the web interface is performed every half of the SystemD `WatchdogSec` defined interval, so if `WatchdogSec=6min` then the health check is performed every 3 minutes. In the case the health check fails, then SystemD watchdog will restart the service automatically.
