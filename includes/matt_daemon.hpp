#ifndef MATT_DAEMON_HPP
#define MATT_DAEMON_HPP

#include "tintin_reporter.hpp"
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
#include <sys/file.h>
#include <sys/epoll.h>
#include <stdexcept>
#include <cerrno>

#define PORT 4242
#define MAX_CLIENTS 3
#define LOCK_FILE "/var/lock/matt_daemon.lock"

class MattDaemon {
public:
	MattDaemon();
	~MattDaemon();

	int createDaemon();
	int createServer();
	void runServer();

private:
	int lockFd;
	int serverFd;
	int epollFd;
	int clientCount;

	void handleNewConnection();
	void handleMessage(int clientFd);
};

#endif
