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

#include <iostream>
#include <exception>

#include "gui/window.h"

using namespace std;
using namespace warsaw;

static void displayGPL3() {
	cout << "  Warsaw, the audio recorder and music composer" << endl;
	cout << "  Copyright(C) 2014 Eigil Hysvær" << endl;
	cout << endl;
	cout << "  This program is free software: you can redistribute it and/or modify" << endl;
	cout << "  it under the terms of the GNU General Public License as published by" << endl;
	cout << "  the Free Software Foundation, either version 3 of the License, or" << endl;
	cout << "  (at your option) any later version." << endl;
	cout << endl;
	cout << "  This program is distributed in the hope that it will be useful," << endl;
	cout << "  but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl;
	cout << "  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the" << endl;
	cout << "  GNU General Public License for more details." << endl;
	cout << endl;
	cout << "  You should have received a copy of the GNU General Public License" << endl;
	cout << "  along with this program. If not, see <http://www.gnu.org/licenses/>." << endl;
	cout << endl;
}

int main(int argc, char** argv) {
	displayGPL3();

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
