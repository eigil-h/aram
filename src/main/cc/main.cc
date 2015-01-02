/*
	ARAM, the audio recorder and music ninja
	Copyright (C) 2014-2015  Eigil Hysvær

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

#include "gui/windowmanager.h"
#include "service/audioengine.h"
#include "service/database.h"

using namespace std;
using namespace aram::gui;
using namespace aram::service;

static void printGPL3() {
	cout << "  ARAM, the audio recorder and music ninja" << endl <<
					"  Copyright(C) 2014 Eigil Hysvær" << endl <<
					endl <<
					"  This program is free software: you can redistribute it and/or modify" << endl <<
					"  it under the terms of the GNU General Public License as published by" << endl <<
					"  the Free Software Foundation, either version 3 of the License, or" << endl <<
					"  (at your option) any later version." << endl <<
					endl <<
					"  This program is distributed in the hope that it will be useful," << endl <<
					"  but WITHOUT ANY WARRANTY; without even the implied warranty of" << endl <<
					"  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the" << endl <<
					"  GNU General Public License for more details." << endl <<
					endl <<
					"  You should have received a copy of the GNU General Public License" << endl <<
					"  along with this program. If not, see <http://www.gnu.org/licenses/>." << endl << endl;
}

int main(int argc, char** argv) {
	printGPL3(); 

	try {
		AudioEngine& audioEngine = AudioEngine::getInstance();
		WindowManagerFactory::assemble(argc, argv)->run();
		cout << "¡hasta luego, amiga!" << endl;
		return 0;
	} catch (std::exception& e) {
		cout << e.what() << endl;
		return -1;
	}
}
