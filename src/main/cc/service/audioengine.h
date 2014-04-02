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


/* Audio system abstraction
 *
 * Implementation of Silence
 */

#ifndef WARSAW_AUDIOENGINE_H
#define WARSAW_AUDIOENGINE_H

#include <memory>
#include <thread>
#include "../model/project.h"
#include "../model/project-odb.hxx"

using namespace std;

namespace warsaw {
	using namespace model;
	namespace service {

		class AudioEngine {
		protected:
			shared_ptr<Project> project;
			unsigned sampleRate;
		public:
			virtual ~AudioEngine();
		};

		class AudioEngineFactory {
		public:
			static unique_ptr<AudioEngine> assemble(int argc, char** argv);
		};

		class Silence : public AudioEngine {
			thread mainTurboThread;
			bool running;
			void mainTurbo();
		public:
			Silence();
			virtual ~Silence();
		};
	}
}


#endif
