#include "matt_daemon.hpp"

MattDaemon::MattDaemon() {
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
