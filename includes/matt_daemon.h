#ifndef MATT_DAEMON_H
#define MATT_DAEMON_H

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <csignal>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define PORT 4242
#define MAX_CLIENTS 3
#define LOG_FILE "/var/log/matt_daemon/matt_daemon.log"
#define LOCK_FILE "/var/lock/matt_daemon.lock"

#endif
