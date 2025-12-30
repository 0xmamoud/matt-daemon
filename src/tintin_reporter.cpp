#include "tintin_reporter.hpp"

TintinReporter& TintinReporter::getInstance() {
	static TintinReporter instance;
	return instance;
}

TintinReporter::TintinReporter() {
	mkdir(LOG_DIR, 0755);
	logFile.open(LOG_FILE, std::ios::app);
}

TintinReporter::~TintinReporter() {
	if (logFile.is_open())
		logFile.close();
}

std::string TintinReporter::getTimestamp() {
	time_t now = time(nullptr);
	struct tm* t = localtime(&now);
	char buf[20];
	strftime(buf, sizeof(buf), "%d/%m/%Y-%H:%M:%S", t);
	return buf;
}

void TintinReporter::writeLog(const std::string& level, const std::string& msg) {
	if (!logFile.is_open())
		return;
	logFile << "[" << getTimestamp() << "] [ " << level << " ] - " << msg << std::endl;
}

void TintinReporter::logLog(const std::string& msg) { writeLog("LOG", msg); }
void TintinReporter::logDebug(const std::string& msg) { writeLog("DEBUG", msg); }
void TintinReporter::logInfo(const std::string& msg) { writeLog("INFO", msg); }
void TintinReporter::logWarn(const std::string& msg) { writeLog("WARN", msg); }
void TintinReporter::logError(const std::string& msg) { writeLog("ERROR", msg); }
void TintinReporter::logFatal(const std::string& msg) { writeLog("FATAL", msg); }

void TintinReporter::log(const std::string& msg) { getInstance().logLog(msg); }
void TintinReporter::debug(const std::string& msg) { getInstance().logDebug(msg); }
void TintinReporter::info(const std::string& msg) { getInstance().logInfo(msg); }
void TintinReporter::warn(const std::string& msg) { getInstance().logWarn(msg); }
void TintinReporter::error(const std::string& msg) { getInstance().logError(msg); }
void TintinReporter::fatal(const std::string& msg) { getInstance().logFatal(msg); }
