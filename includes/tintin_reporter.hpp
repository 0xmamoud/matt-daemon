#ifndef TINTIN_REPORTER_HPP
#define TINTIN_REPORTER_HPP

#include <string>
#include <fstream>
#include <ctime>
#include <sys/stat.h>

#define LOG_DIR "/var/log/matt_daemon"
#define LOG_FILE "/var/log/matt_daemon/matt_daemon.log"

class TintinReporter {
public:
	static void log(const std::string& msg);
	static void debug(const std::string& msg);
	static void info(const std::string& msg);
	static void warn(const std::string& msg);
	static void error(const std::string& msg);
	static void fatal(const std::string& msg);

	TintinReporter(const TintinReporter&) = delete;
	TintinReporter& operator=(const TintinReporter&) = delete;

private:
	static TintinReporter& getInstance();

	TintinReporter();
	~TintinReporter();

	void logLog(const std::string& msg);
	void logDebug(const std::string& msg);
	void logInfo(const std::string& msg);
	void logWarn(const std::string& msg);
	void logError(const std::string& msg);
	void logFatal(const std::string& msg);

	void		writeLog(const std::string& level, const std::string& msg);
	std::string	getTimestamp();

	std::ofstream logFile;
};

#endif
