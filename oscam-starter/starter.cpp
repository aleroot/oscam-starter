//
//  starter.cpp
//  oscam-starter
//
//  Created by Alessio Pollero on 29/04/21.
//

#include "starter.hpp"
#include "utils.hpp"
#include "config.hpp"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

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
    char * argv[args.size() + 1];
    for(int i = 0; i < args.size(); i++) {
        argv[i] = (char *)args[i];
    }
    argv[args.size()] = NULL;
    
    
    pid_oscam = fork(); /* Create a child process */

    switch (pid_oscam) {
        case -1: /* Error */
            std::cerr << "Cannot start OScam. fork() failed!\n";
            exit(1);
        case 0: /* Child process */
            execvp(path.c_str(), argv); /* Execute the program */
            std::cerr << "Cannot start OScam. execl() failed!\n"; /* execl doesn't return unless there's an error */
            exit(1);
        default: /* Parent process */
            std::cout << "Process created with pid: " << pid_oscam << "\n";
            int status;

            //while (!WIFEXITED(status)) {
                waitpid(pid_oscam, &status, 0); /* Wait for the process to complete */
                //if(kill(pid, 0) == ESRCH) break;
            //}

            std::cout << "Process with pid: " << pid_oscam << " exited with status: " << WEXITSTATUS(status) << "\n";

            return status;
    }
}

int Starter::start() {
    return start(std::vector < const char * >());
}
