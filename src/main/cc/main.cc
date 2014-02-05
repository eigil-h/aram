#include <iostream>
#include <gtkmm.h>

using namespace std;

namespace warsaw {
class Window : public Gtk::Window {
public:

	Window() {
		set_default_size(400, 200);
		set_title("Warsaw");
		set_position(Gtk::WindowPosition::WIN_POS_CENTER);
		show_all_children();
	}
};
}

int main(int argc, char** argv) {
	Glib::RefPtr<Gtk::Application> application = Gtk::Application::create(argc, argv, "warsaw.app");
	warsaw::Window window;
	int result = application->run(window);

	cout << "see you later, amiga!" << endl;
	return result;
}
