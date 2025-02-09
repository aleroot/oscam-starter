//
//  starter.cpp
//  oscam-starter
//
//  Created by Alessio Pollero on 29/04/21.
//

#include "starter.hpp"
#include "utils.hpp"
#include "config.hpp"
#include <chrono>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

static const std::chrono::minutes RESTART_WINDOW(5);
pid_t pid_oscam;

void kill_oscam(int sig)
{
    std::cout << "Killing OScam process with pid " << pid_oscam << "\n";
    killpg(getpgid(pid_oscam),SIGKILL);
}

void reload_oscam(int sig)
{
    std::cout << "Reloading OScam process with pid " << pid_oscam << "\n";
    kill(pid_oscam,SIGHUP);
}

Starter::Starter(std::string e_path) {
    path = e_path;
}

Starter::Starter() : Starter(which(OSCAM)) {};

int Starter::start(std::vector < const char * > args) {
    auto last_restart = std::chrono::steady_clock::now() - RESTART_WINDOW;
    char * argv[args.size() + 1];
    for(int i = 0; i < args.size(); i++) {
        argv[i] = const_cast<char*>(args[i]);
    }
    argv[args.size()] = NULL;
    
    while((std::chrono::steady_clock::now() - last_restart) >= RESTART_WINDOW) {
        last_restart = std::chrono::steady_clock::now();
        pid_oscam = fork(); /* Create a child process */
        
        switch (pid_oscam) {
            case -1: /* Error */
                std::cerr << "Cannot start OScam. fork() failed!\n";
                exit(1);
            case 0: /* Child process */
                execvp(path.c_str(), argv); /* Execute the program(OSCam) */
                std::cerr << "Cannot start OScam. execvp() failed! Error: " << strerror(errno) << "\n"; /* execl doesn't return unless there's an error */
                exit(1);
            default: /* Parent process */
                std::cout << "Process created with pid: " << pid_oscam << "\n";
                int status;
                if (waitpid(pid_oscam, &status, 0) == -1) /* Wait for the process to complete */
                    return 2; /* waitpid call failed */
                std::cout << "Process with pid: " << pid_oscam << " exited with status: " << WEXITSTATUS(status) << "\n";
                if(WIFEXITED(status) && WEXITSTATUS(status) == 0) {
                    std::cout << "Attempting restart...\n";
                    continue;
                }
                
                return status;
        }
    }
    return 1;
}

int Starter::start() {
    return start(std::vector < const char * >());
}
