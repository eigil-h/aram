#include "window.h"

warsaw::gui::Window::Window() {
	set_default_size(1200, 750);
	set_title("Warsaw");
	set_position(Gtk::WindowPosition::WIN_POS_CENTER);
	show_all_children();
}

warsaw::GtkmmApplication::GtkmmApplication(int argc, char** argv) {
	app = Gtk::Application::create(argc, argv, "warsaw.app");
}

void warsaw::GtkmmApplication::run() throw (std::exception) {
	gui::Window window;
	int result = app->run(window);

	if (result != 0) {
		throw std::exception();
	}
}
