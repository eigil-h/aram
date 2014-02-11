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
