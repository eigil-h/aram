/*
	ARAM, the audio recorder and music ninja
	Copyright (C) 2014  Eigil Hysvær

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WARZAW_WINDOWMANAGER_H
#define WARZAW_WINDOWMANAGER_H

#include <memory>
#include <gtkmm.h>

using namespace std;

namespace aram {
	/** 
	 * Graphical user interface name space
	 */
	namespace gui {
/*
		 * Below are classes for window/application management
		 * Central is the WindowManager.run() method. By calling it, the control flow of the application
		 * will be handed over to the window manager. Any custom code will run as callbacks given to the
		 * manager system, eg. Gtkmm.
		 */
		class WindowManager {
		public:
			virtual void run() throw (std::exception) = 0;
		};

		class WindowManagerFactory {
		public:
			static unique_ptr<WindowManager> assemble(int argc, char** argv);
		};

		class GtkmmApplication : public WindowManager {
			Glib::RefPtr<Gtk::Application> app;

		public:
			GtkmmApplication(int argc, char** argv);

			virtual void run() throw (std::exception);
		};
	}
}

#endif
