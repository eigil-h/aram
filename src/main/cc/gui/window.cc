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

#include "window.h"
#include <iostream>

warsaw::gui::Window::Window() {
	Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
	Glib::RefPtr<Gtk::StyleContext> refStyleContext = get_style_context();
	Glib::RefPtr< Gdk::Screen > defaultScreen = Gdk::Screen::get_default();
	refStyleContext->add_provider_for_screen(defaultScreen, cssProvider,
					GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	try {
		cssProvider->load_from_path("style.css");

		set_default_size(1200, 750);
		set_title("Warsaw");
		set_position(Gtk::WindowPosition::WIN_POS_CENTER);

		projectContainer.set_size_request(-1, 325);
		audioclipContainer.set_size_request(-1, 325);

		mainContainer.pack_start(projectContainer);
		mainContainer.pack_start(audioclipContainer);
		add(mainContainer);

		show_all_children();

	} catch (const Glib::Error& e) {
		std::cerr << e.what() << std::endl;
		throw std::exception();
	}
}

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

warsaw::gui::ProjectContainer::ProjectContainer()
: tmp("Project view") {

	Gtk::Image* image = Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_PLAY,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	image->show();
	playButton.set_image(*image);
	playButton.set_size_request(200, -1);

	tmp.set_size_request(1000, -1);

	pack_start(playButton);
	pack_start(tmp);
}

warsaw::gui::AudioclipContainer::AudioclipContainer()
: tmp("Audioclip view") {

	Gtk::Image* recordImage = Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_RECORD,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	recordImage->show();

	recordButton.set_image(*recordImage);
	recordButton.set_size_request(-1, 325 / 2);

	Gtk::Image* markImage = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_ALL,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	markImage->show();
	markButton.set_image(*markImage);
	markButton.set_size_request(-1, 325 / 2);

	buttonContainer.set_size_request(200, -1);
	tmp.set_size_request(1000, -1);

	buttonContainer.pack_start(recordButton);
	buttonContainer.pack_start(markButton);

	pack_start(buttonContainer);
	pack_start(tmp);
}
