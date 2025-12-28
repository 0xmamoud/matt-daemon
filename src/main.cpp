#include "matt_daemon.h"
#include "tintin_reporter.hpp"


int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
		TintinReporter::info("Testing TintinReporter info log.");
		TintinReporter::debug("Testing TintinReporter debug log.");
		TintinReporter::warn("Testing TintinReporter warn log.");
		TintinReporter::error("Testing TintinReporter error log.");
		TintinReporter::fatal("Testing TintinReporter fatal log.");

    
    return 0;
}
