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

#include "audioengine.h"
#include <cstring>
#include "jackclient.h"

static bool isSilence(int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-silence") == 0) {
			return true;
		}
	}
	return false;
}

std::unique_ptr<warsaw::service::AudioEngine> warsaw::service::AudioEngineFactory::assemble(int argc, char** argv) {
	AudioEngine* as;
	if (isSilence(argc, argv)) {
		as = new Silence();
	} else {
		as = new JackClient();
	}
	std::unique_ptr<AudioEngine> asp(as);
	return asp;
}

warsaw::service::AudioEngine::~AudioEngine() {

}