/*
 * File:   application.h
 * Author: eigil
 *
 * Created on February 5, 2014, 1:20 PM
 */
#include <exception>

#ifndef WARZAW_APPLICATION_H
#define WARZAW_APPLICATION_H

namespace warsaw {

	class Application {
	public:
		virtual void run() throw (std::exception) = 0;
	};
}
#endif
