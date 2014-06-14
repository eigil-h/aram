/*
	Warsaw, the audio recorder and music composer
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

#include "windowmanager.h"
#include "window.h"

std::unique_ptr<warsaw::gui::WindowManager> warsaw::gui::WindowManagerFactory::assemble(int argc, char** argv) {
	//To prevent gtk to freak out when program is given eg -silence argument, argc is set to 1
	std::unique_ptr<WindowManager> app(new GtkmmApplication(1, argv));
	return app;
}

warsaw::gui::GtkmmApplication::GtkmmApplication(int argc, char** argv) {
	app = Gtk::Application::create(argc, argv, "warsaw.app");
}

void warsaw::gui::GtkmmApplication::run() throw (std::exception) {
	gui::Window window;
	int result = app->run(window);

	if (result != 0) {
		throw std::exception();
	}
}
