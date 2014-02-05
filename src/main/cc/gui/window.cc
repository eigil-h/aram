#include "window.h"

#define BUTTON_BORDER_WIDTH 10

warsaw::gui::Window::Window() {
	set_default_size(1200, 750);
	set_title("Warsaw");
	set_position(Gtk::WindowPosition::WIN_POS_CENTER);

	projectContainer.set_size_request(-1, 325);
	audioclipContainer.set_size_request(-1, 325);

	mainContainer.pack_start(projectContainer);
	mainContainer.pack_start(audioclipContainer);
	add(mainContainer);

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

warsaw::gui::ProjectContainer::ProjectContainer()
: playButton("Play")
, tmp("Project view") {
	playButton.set_size_request(200, -1);
	playButton.set_border_width(BUTTON_BORDER_WIDTH);

	tmp.set_size_request(1000, -1);

	pack_start(playButton);
	pack_start(tmp);
}

warsaw::gui::AudioclipContainer::AudioclipContainer()
: recordButton("Record")
, markButton("Mark")
, tmp("Audioclip view") {
	recordButton.set_size_request(-1, 325 / 2);
	recordButton.set_border_width(BUTTON_BORDER_WIDTH);
	markButton.set_size_request(-1, 325 / 2);
	markButton.set_border_width(BUTTON_BORDER_WIDTH);
	buttonContainer.set_size_request(200, -1);
	tmp.set_size_request(1000, -1);

	buttonContainer.pack_start(recordButton);
	buttonContainer.pack_start(markButton);

	pack_start(buttonContainer);
	pack_start(tmp);
}
