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

#include <cstring>
#include <chrono>
#include <algorithm>
#include "audioengine.h"
#include "jackclient.h"
#include "database.h"
#include "system.h"

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * AudioEngine
 */
aram::service::AudioEngine& aram::service::AudioEngine::getInstance() {
	static unique_ptr<AudioEngine> asp(newAudioEngine());
	return *asp;
}

aram::service::AudioEngine* aram::service::AudioEngine::newAudioEngine() {
	for (string s : aram::System::getProgramArguments()) {
		if (s == "-silence") {
			return new SilenceAdaptedAudioEngine();
		}
	}
	return new JackAdaptedAudioEngine();
}

aram::service::AudioEngine::AudioEngine() :
				backBufferThread(&AudioEngine::backBufferTurbo, this),
				backBufferRunning(true) {
}

aram::service::AudioEngine::~AudioEngine() {
	backBufferRunning = false;
	this_thread::sleep_for(chrono::milliseconds(1100));
	backBufferThread.join();
}

void aram::service::AudioEngine::backBufferTurbo() {
	while(backBufferRunning) {
		this_thread::sleep_for(chrono::milliseconds(1000));

		if (playback) {
			if (recorder.get() != nullptr) {
				recorder->swapAndStore();
			}
		}
		
		//for each channel - loadBackBuffer
	}
}

void aram::service::AudioEngine::addChannel(const string& channel) {
	channels.push_front(channel);
}

void aram::service::AudioEngine::removeChannel(const string& channel) {

}

void aram::service::AudioEngine::armChannel(const string& channel) {
	recorder.reset(new Recorder(channel));
}

void aram::service::AudioEngine::disarmChannel() {
	recorder.reset();
}


/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Recorder
 */
aram::service::Recorder::Recorder(const string& channel_) :
				channel(channel_),
				recordingBufferLeft(491520),
				recordingBufferRight(491520),
				recordingStreamLeft((System::getHomePath() + "/.aram/" + channel + "-l").c_str(),
								ios_base::binary | ios_base::trunc),
				recordingStreamRight((System::getHomePath() + "/.aram/" + channel + "-r").c_str(),
								ios_base::binary | ios_base::trunc) {
}



bool aram::service::Recorder::record(Samples left, Samples right, unsigned count) {
	if (!recordingBufferLeft.writeFrontBuffer(left, count)) {
		return false;
	}
	if (!recordingBufferRight.writeFrontBuffer(right, count)) {
		return false;
	}
	return true;
}

void aram::service::Recorder::swapAndStore() {
	recordingBufferLeft.swapAndStoreBackBuffer(recordingStreamLeft);
	recordingBufferRight.swapAndStoreBackBuffer(recordingStreamRight);
}


/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Jack adapted audio engine
 */
static int onFrameReadyJackFun(uint32_t frameCount, void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngine::getInstance();
	audioEngine.frameReadySignal(frameCount);
	return JACK_CALLBACK_SUCCESS;
}

static int onXRunJackFun(void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngine::getInstance();
	audioEngine.xRunSignal();
	return JACK_CALLBACK_SUCCESS;
}

static int onSampleRateChangeJackFun(unsigned sampleRate, void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngine::getInstance();
	audioEngine.sampleRateChangeSignal(sampleRate);
	return JACK_CALLBACK_SUCCESS;
}

static void onShutdownJackFun(void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngine::getInstance();
	audioEngine.shutdownSignal();
}

static void onErrorJackFun(const char* msg) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngine::getInstance();
	audioEngine.errorSignal(msg);
}

aram::service::JackAdaptedAudioEngine::JackAdaptedAudioEngine() {
}

void aram::service::JackAdaptedAudioEngine::init() {
	jack_set_error_function(onErrorJackFun);

	jack_status_t status;
	jackClient = jack_client_open("aram", JackNullOption, &status);
	if (jackClient == nullptr) {
		throw runtime_error("Jack server is not running.");
	}

	jack_set_process_callback(jackClient, onFrameReadyJackFun, this);
	jack_set_xrun_callback(jackClient, onXRunJackFun, this);
	jack_set_sample_rate_callback(jackClient, onSampleRateChangeJackFun, this);
	jack_on_shutdown(jackClient, onShutdownJackFun, this);

	physicalInputPort.registerPort(jackClient, DIRECTION_INPUT, "capture");
	physicalOutputPort.registerPort(jackClient, DIRECTION_OUTPUT, "playback");

	int errorCode = jack_activate(jackClient);
	if (errorCode != 0) {
		throw runtime_error("Jack activation failed with error " + errorCode);
	}

	physicalInputPort.connectPhysicalPort(jackClient, DIRECTION_INPUT);
	physicalOutputPort.connectPhysicalPort(jackClient, DIRECTION_OUTPUT);

	frameReadySignal.connect(sigc::mem_fun(this, &JackAdaptedAudioEngine::onFrameReady));
}

aram::service::JackAdaptedAudioEngine::~JackAdaptedAudioEngine() {
	if (jackClient != nullptr) {
		jack_client_close(jackClient);
	}
}

void aram::service::JackAdaptedAudioEngine::onFrameReady(unsigned frameCount) {
	Samples leftIn = reinterpret_cast<Samples> (jack_port_get_buffer(
					physicalInputPort.ports[CHANNEL_LEFT], frameCount));
	Samples rightIn = reinterpret_cast<Samples> (jack_port_get_buffer(
					physicalInputPort.ports[CHANNEL_RIGHT], frameCount));
	Samples leftOut = reinterpret_cast<Samples> (jack_port_get_buffer(
					physicalOutputPort.ports[CHANNEL_LEFT], frameCount));
	Samples rightOut = reinterpret_cast<Samples> (jack_port_get_buffer(
					physicalOutputPort.ports[CHANNEL_RIGHT], frameCount));

	//unconditionally copy input buffer to output buffer for immediate playback
	::memcpy(leftOut, leftIn, sizeof (Sample) * frameCount);
	::memcpy(rightOut, rightIn, sizeof (Sample) * frameCount);

	if (playback) {

		for_each(channels.begin(), channels.end(), [](string & channel) {
			//get read audio buffer mapped to the channel
			//get the port mapped to the channel
			//copy audio buffer frames to the port
		});

		if (recorder.get() != nullptr) {
			if(!recorder->record(leftIn, rightIn, frameCount)) {
				xRunSignal(); //todo - maybe a parameter to explain it's not really an xrun...?
			}
		}
	}
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Silence adapted audio engine
 */
aram::service::SilenceAdaptedAudioEngine::SilenceAdaptedAudioEngine() : 
				mainTurboThread(&SilenceAdaptedAudioEngine::mainTurbo, this),
				running(true), frameCountPlayback(0), frameCountRecording(0),
				frameCountTotal(0) {
	cout << "Constructing the Silence Adapted Audio Engine" << endl;
}

void aram::service::SilenceAdaptedAudioEngine::init() {
	frameReadySignal.connect(sigc::mem_fun(this, &SilenceAdaptedAudioEngine::onFrameReady));
}

aram::service::SilenceAdaptedAudioEngine::~SilenceAdaptedAudioEngine() {
	cout << "Destroying the Silence Adapted Audio Engine" << endl;

	running = false;
	this_thread::sleep_for(chrono::milliseconds(100));

	cout << "frame count played back = " << frameCountPlayback << endl;
	cout << "frame count recorded = " << frameCountRecording << endl;
	cout << "frame count total = " << frameCountTotal << endl;

	mainTurboThread.join();
}

void aram::service::SilenceAdaptedAudioEngine::mainTurbo() {
	while (running) {
		frameReadySignal(1);
		this_thread::sleep_for(chrono::milliseconds(50));
	}
}

void aram::service::SilenceAdaptedAudioEngine::onFrameReady(unsigned frameCount) {
	frameCountTotal += frameCount;
	if (playback) {
		frameCountPlayback += frameCount;

		if (recorder.get() != nullptr) {
			frameCountRecording += frameCount;
		}
	}
}
