/*
 * File:   window.h
 * Author: eigil
 *
 * Created on February 5, 2014, 1:14 PM
 */
#include <gtkmm.h>
#include "../application.h"

#ifndef WINDOW_H
#define	WINDOW_H

namespace warsaw {
	namespace gui {

		class Window : public Gtk::Window {
		public:

			Window();
		};
	}

	class GtkmmApplication : public Application {
		Glib::RefPtr<Gtk::Application> app;

	public:
		GtkmmApplication(int argc, char** argv);

		virtual void run() throw (std::exception);
	};
}

#endif	/* WINDOW_H */

