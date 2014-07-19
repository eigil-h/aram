/*
	ARAM, the audio recorder and music ninja
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


#ifndef ARAM_AUDIOENGINE_H
#define ARAM_AUDIOENGINE_H

#include <memory>
#include <thread>
#include <unordered_set>
#include <jack/jack.h>
#include <sigc++/sigc++.h>

using namespace std;

namespace aram {
	namespace service {

		class AudioEngine {
		public:
			static AudioEngine& getInstance();

			virtual void start() = 0;
			virtual void stop() = 0;

			sigc::signal<void, unsigned> frameReadySignal;
			sigc::signal<void> xRunSignal;
			sigc::signal<void, unsigned> sampleRateChangeSignal;
			sigc::signal<void> shutdownSignal;
			sigc::signal<void, const char*> errorSignal;

		protected:
			AudioEngine();

		private:
			static AudioEngine* newAudioEngine();

			AudioEngine(const AudioEngine&) = delete;
			AudioEngine& operator=(const AudioEngine&) = delete;
		};

		class JackAdaptedAudioEngine : public AudioEngine {
		public:
			JackAdaptedAudioEngine();
			~JackAdaptedAudioEngine();

			void start();
			void stop();

		private:
			jack_client_t* jackClient;
		};

		class SilenceAdaptedAudioEngine : public AudioEngine {
		public:
			SilenceAdaptedAudioEngine();
			~SilenceAdaptedAudioEngine();

			void start();
			void stop();

		private:
			thread mainTurboThread;
			bool running;
			unsigned frameCount_;

			void mainTurbo();
			void onFrameReady(unsigned frameCount);
		};
	}
}


#endif
