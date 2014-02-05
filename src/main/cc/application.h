/*
 * File:   application.h
 * Author: eigil
 *
 * Created on February 5, 2014, 1:20 PM
 */
#include <exception>

#ifndef APPLICATION_H
#define	APPLICATION_H

namespace warsaw {

	class Application {
	public:
		virtual void run() throw (std::exception) = 0;
	};
}
#endif	/* APPLICATION_H */

