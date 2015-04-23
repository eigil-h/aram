#include "controller.h"

aram::service::Controller& aram::service::Controller::getInstance() {
	static Controller instance;
	return instance;
}
