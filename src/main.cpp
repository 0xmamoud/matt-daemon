#include "matt_daemon.hpp"
#include "tintin_reporter.hpp"
#include "sighandler.hpp"

void checkRoot() {
	if (getuid() != 0) {
		TintinReporter::error("Program must be run as root");
		throw std::runtime_error("Not root");
	}
}

int main(void) {
	try {
		checkRoot();
		MattDaemon daemon;
		if (daemon.createDaemon() < 0) {
			std::cerr << "Failed to create daemon" << std::endl;
			return 1;
		}	

		SignalHandler::setup();

		// TODO: server loop

	} catch (const std::exception& e) {
		TintinReporter::fatal(std::string("Matt Daemon ") + e.what());
		return 1;
	}

	return 0;
}
