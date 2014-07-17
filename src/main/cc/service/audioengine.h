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

using namespace std;

//replace with sigc++
typedef void (*void_int_cb)(int);
typedef void (*void_void_cb)();
typedef void (*void_constcharstar_cb)(const char*);

namespace aram {
	namespace service {
		class AudioEngine {
		public:
			AudioEngine();
			AudioEngine(const AudioEngine&) = delete;
			AudioEngine& operator=(const AudioEngine&) = delete;

			void addFrameReadyObserver(void_int_cb);
			void addXRunObserver(void_void_cb);
			void addSampleRateChangeObserver(void_int_cb);
			void addShutdownObserver(void_void_cb);
			void addErrorObserver(void_constcharstar_cb);

			virtual void start() = 0;
			virtual void stop() = 0;

			int postOnFrameReady(unsigned frameCount);
			int onXRun();
			int onSampleRateChange(unsigned sampleRate);
			void onShutdown();
			void onError(const char* msg);
			
			unordered_set<void_int_cb> onFrameReadyObserver;
			unordered_set<void_void_cb> onXRunObserver;
			unordered_set<void_int_cb> onSampleRateChangeObserver;
			unordered_set<void_void_cb> onShutdownObserver;
			unordered_set<void_constcharstar_cb> onErrorObserver;
		};

		class AudioEngineFactory {
		public:
			static AudioEngine& audioEngine();
		private:
			static AudioEngine* assemble();
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
			int onFrameReady(unsigned frameCount);
		};
	}
}


#endif
