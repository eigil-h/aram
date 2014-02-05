#include <iostream>
#include <exception>

#include "gui/window.h"

using namespace std;
using namespace warsaw;

int main(int argc, char** argv) {
	GtkmmApplication app(argc, argv);

	try {
		app.run();
		cout << "see you later, amiga!" << endl;
		return 0;
	} catch (std::exception e) {
		cout << e.what() << endl;
		return -1;
	}
}
