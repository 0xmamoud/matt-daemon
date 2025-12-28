#include "matt_daemon.h"
#include "tintin_reporter.hpp"

void checkRoot() {
	if (getuid() != 0) {
		TintinReporter::error("Program must be run as root");
		throw std::runtime_error("Not root");
	}
}

int acquireLock() {
	int fd = open(LOCK_FILE, O_CREAT | O_RDWR, 0644);
	if (fd < 0) {
		TintinReporter::error("Cannot open lock file");
		throw std::runtime_error("Cannot open lock file");
	}
	if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
		TintinReporter::error("Daemon already running (lock file locked)");
		close(fd);
		throw std::runtime_error("Already running");
	}
	return fd;
}

void releaseLock(int fd) {
	flock(fd, LOCK_UN);
	close(fd);
	unlink(LOCK_FILE);
}

int main(void) {

	try {
		checkRoot();
		int lockFd = acquireLock();
		TintinReporter::info("Matt_daemon started");

		// TODO: daemon logic

		releaseLock(lockFd);
		TintinReporter::info("Matt_daemon stopped");
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		TintinReporter::fatal(std::string("Fatal error: ") + e.what());
		return 1;
	}

	return 0;
}
