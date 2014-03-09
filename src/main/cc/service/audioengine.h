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

namespace warsaw {
	namespace service {

		class AudioEngine {
		public:
			virtual ~AudioEngine() = 0;
		};

		class AudioEngineFactory {
		public:
			static std::unique_ptr<AudioEngine> assemble(int argc, char** argv);
		};

		class Silence : public AudioEngine {
		};
	}
}


#endif
