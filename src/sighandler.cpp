#include "sighandler.hpp"
#include "tintin_reporter.hpp"

volatile sig_atomic_t SignalHandler::running = 1;

void SignalHandler::setup() {
	struct sigaction sa;
	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	int signals[] = { SIGINT, SIGTERM, SIGQUIT, SIGHUP, SIGPIPE };

	for (int sig : signals) {
		sigaction(sig, &sa, nullptr);
	}
	TintinReporter::info("Signal handler initialized");
}

bool SignalHandler::isRunning() {
	return running == 1;
}

void SignalHandler::stop() {
	running = 0;
}

void SignalHandler::handler(int sig) {
	TintinReporter::info("Signal received: " + signalName(sig));
	running = 0;
}

std::string SignalHandler::signalName(int sig) {
	switch (sig) {
		case SIGINT:  return "SIGINT";
		case SIGTERM: return "SIGTERM";
		case SIGQUIT: return "SIGQUIT";
		case SIGHUP:  return "SIGHUP";
		case SIGPIPE: return "SIGPIPE";
		default:      return std::to_string(sig);
	}
}
