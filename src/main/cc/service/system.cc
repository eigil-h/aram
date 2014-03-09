#include "system.h"
#include <cstdlib>

const string warsaw::System::getHomePath() {
	return ::getenv("HOME");
}
