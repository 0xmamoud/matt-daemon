#ifndef SIGHANDLER_HPP
#define SIGHANDLER_HPP

#include <csignal>
#include <string>

class SignalHandler {
public:
	static void setup();
	static bool isRunning();
	static void stop();

private:
	static volatile sig_atomic_t running;
	static void handler(int sig);
	static std::string signalName(int sig);
};

#endif