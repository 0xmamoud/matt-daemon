#include "matt_daemon.hpp"
#include "sighandler.hpp"

MattDaemon::MattDaemon() : serverFd(-1), epollFd(-1) {
	lockFd = open(LOCK_FILE, O_CREAT | O_RDWR, 0644);
	if (lockFd < 0) {
		throw std::runtime_error("Cannot open lock file");
	}
	if (flock(lockFd, LOCK_EX | LOCK_NB) < 0) {
		close(lockFd);
		throw std::runtime_error("Daemon already running");
	}
	TintinReporter::info("Matt Daemon Lock acquired");
}

MattDaemon::~MattDaemon() {
	TintinReporter::info("Matt Daemon exiting");
	if (epollFd >= 0)
		close(epollFd);
	if (serverFd >= 0)
		close(serverFd);
	flock(lockFd, LOCK_UN);
	close(lockFd);
	unlink(LOCK_FILE);
}

int MattDaemon::createDaemon() {
	TintinReporter::info("Entering daemon mode");

	pid_t pid = fork();
	if (pid < 0) {
		TintinReporter::error("First fork failed");
		return -1;
	}
	if (pid > 0)
		exit(EXIT_SUCCESS);

	setsid();

	pid = fork();
	if (pid < 0) {
		TintinReporter::error("Second fork failed");
		return -1;
	}
	if (pid > 0)
		exit(EXIT_SUCCESS);

	chdir("/");
	umask(0);

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	TintinReporter::info("Daemon started, PID: " + std::to_string(getpid()));
	return 0;
}

int MattDaemon::createServer() {
	serverFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (serverFd < 0) {
		TintinReporter::error("Socket creation failed");
		return -1;
	}

	int opt = 1;
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		TintinReporter::error("setsockopt failed");
		return -1;
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(PORT);

	if (bind(serverFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		TintinReporter::error("Bind failed");
		return -1;
	}

	if (listen(serverFd, MAX_CLIENTS) < 0) {
		TintinReporter::error("Listen failed");
		return -1;
	}

	epollFd = epoll_create1(0);
	if (epollFd < 0) {
		TintinReporter::error("epoll_create1 failed");
		return -1;
	}

	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = serverFd;
	if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &ev) < 0) {
		TintinReporter::error("epoll_ctl failed");
		return -1;
	}

	TintinReporter::info("Server created on port " + std::to_string(PORT));
	return 0;
}

void MattDaemon::runServer() {
	struct epoll_event events[MAX_CLIENTS + 1];

	TintinReporter::info("Server started");

	while (SignalHandler::isRunning()) {
		int nfds = epoll_wait(epollFd, events, MAX_CLIENTS + 1, 1000);
		if (nfds < 0) {
			if (errno == EINTR)
				continue;
			TintinReporter::error("epoll_wait failed");
			break;
		}

		for (int i = 0; i < nfds; i++) {
			if (events[i].data.fd == serverFd) {
				// TODO: handleNewConnection()
			} else {
				// TODO: handleMessage(events[i].data.fd)
			}
		}
	}

	TintinReporter::info("Server stopped");
}
