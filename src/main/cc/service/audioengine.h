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


#ifndef ARAM_AUDIOENGINE_H
#define ARAM_AUDIOENGINE_H

#include <forward_list>
#include <thread>
#include <jack/jack.h>
#include <sigc++/sigc++.h>
#include "jackclient.h"
#include "buffers.h"

using namespace std;

namespace aram {
	/**
	 * Services that knows nothing about the models.
	 */
	namespace service {

		/**
		 * Abstract audio engine with concrete signal storage.
		 * Subclasses make real representation of audio engines like JACK.
		 * Implemented as singleton because we always want one and only one
		 * audio engine instance. This way it becomes easy for C style audio engine
		 * callbacks to access the signal storage contained here.
		 *
		 * !..Keep in mind: possible concurrency issues while changing state..!
		 */
		class AudioEngine {
		public:
			static AudioEngine& getInstance();

			sigc::signal<void, unsigned> frameReadySignal;
			sigc::signal<void> xRunSignal;
			sigc::signal<void, unsigned> sampleRateChangeSignal;
			sigc::signal<void> shutdownSignal;
			sigc::signal<void, const char*> errorSignal;

			bool playback;

			//jack callbacks are using getInstance for setting the signals,
			//so we can't call jack_set_sample_rate_callback from the constructor.
			//Instead we call it from this one.
			virtual void init() = 0;

			virtual ~AudioEngine();

			void addChannel(const string& channel);
			void removeChannel(const string& channel);
			void armChannel(const string& channel);

		protected:
			AudioEngine(unsigned recordingBufferLen);

			forward_list<string> channels;
			string armedChannel;
			WriteAndStoreBuffer recordingBufferLeft, recordingBufferRight;

		private:
			static AudioEngine* newAudioEngine();

			AudioEngine(const AudioEngine&) = delete;
			AudioEngine& operator=(const AudioEngine&) = delete;


			thread backBufferThread;
			bool backBufferRunning;
			void backBufferTurbo();
		};

		/**
		 * Audio engine using JACK
		 */
		class JackAdaptedAudioEngine : public AudioEngine {
#define JACK_CALLBACK_SUCCESS 0;

		public:
			JackAdaptedAudioEngine();
			~JackAdaptedAudioEngine();

			void init();

		private:
			jack_client_t* jackClient;
			JackStereoPort physicalInputPort;
			JackStereoPort physicalOutputPort;

			void onFrameReady(unsigned frameCount);
		};

		/**
		 * Audio engine without audio. Useful for GUI design work.
		 */
		class SilenceAdaptedAudioEngine : public AudioEngine {
		public:
			SilenceAdaptedAudioEngine();
			~SilenceAdaptedAudioEngine();

			void init();

		private:
			thread mainTurboThread;
			bool running;
			unsigned frameCountPlayback,
			frameCountRecording,
			frameCountTotal;

			void mainTurbo();
			void onFrameReady(unsigned frameCount);
		};
	}
}


#endif
