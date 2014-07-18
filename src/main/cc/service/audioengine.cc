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

#include <iostream>
#include <cstring>
#include <chrono>
#include "audioengine.h"
#include "jackclient.h"
#include "database.h"
#include "system.h"

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * AudioEngineFactory
 */
aram::service::AudioEngine& aram::service::AudioEngineFactory::audioEngine() {
	static unique_ptr<AudioEngine> asp(assemble());
	return *asp;
}

aram::service::AudioEngine* aram::service::AudioEngineFactory::assemble() {
	for (string s : aram::System::getProgramArguments()) {
		if (s == "-silence") {
			return new SilenceAdaptedAudioEngine();
		}
	}
	//todo new JackAdaptedAudioEngine();
	return new SilenceAdaptedAudioEngine();
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * AudioEngine
 */
aram::service::AudioEngine::AudioEngine() {
}

void aram::service::AudioEngine::connectFrameReadyObserver(sigc::slot<void, unsigned> slot) {
	frameReadySignal.connect(slot);
}

void aram::service::AudioEngine::connectXRunObserver(sigc::slot<void> slot) {
	xRunSignal.connect(slot);
}

void aram::service::AudioEngine::connectSampleRateChangeObserver(sigc::slot<void, unsigned> slot) {
	sampleRateChangeSignal.connect(slot);
}

void aram::service::AudioEngine::connectShutdownObserver(sigc::slot<void> slot) {
	shutdownSignal.connect(slot);
}

void aram::service::AudioEngine::connectErrorObserver(sigc::slot<void, const char*> slot) {
	errorSignal.connect(slot);
}

void aram::service::AudioEngine::emitFrameReady(unsigned frameCount) {
	frameReadySignal.emit(frameCount);
}

void aram::service::AudioEngine::emitXRun() {
	xRunSignal.emit();
}

void aram::service::AudioEngine::emitSampleRateChange(unsigned sampleRate) {
	sampleRateChangeSignal.emit(sampleRate);
}

void aram::service::AudioEngine::emitShutdown() {
	shutdownSignal.emit();
}

void aram::service::AudioEngine::emitError(const char* msg) {
	errorSignal.emit(msg);
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Jack adapted audio engine
 */
static int onFrameReadyJackFun(uint32_t frameCount, void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.emitFrameReady(frameCount);
}

static int onXRunJackFun(void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.emitXRun();
}

static int onSampleRateChangeJackFun(unsigned sampleRate, void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.emitSampleRateChange(sampleRate);
}

static void onShutdownJackFun(void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.emitShutdown();
}

static void onErrorJackFun(const char* msg) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.emitError(msg);
}

aram::service::JackAdaptedAudioEngine::JackAdaptedAudioEngine() {
	jack_set_error_function(onErrorJackFun);

	jack_status_t status;
	jackClient = jack_client_open("aram", JackNullOption, &status);
	if (jackClient == nullptr) {
		cout << "Jack server is not running." << endl;
		throw exception();
	}

	jack_set_process_callback(jackClient, onFrameReadyJackFun, this);
	jack_set_xrun_callback(jackClient, onXRunJackFun, this);
	jack_set_sample_rate_callback(jackClient, onSampleRateChangeJackFun, this);
	jack_on_shutdown(jackClient, onShutdownJackFun, this);

	/*registerPort
	 * jack_activate
	 * port connect
	 *  - guess this should go into some JackClient class.
	 */
}

aram::service::JackAdaptedAudioEngine::~JackAdaptedAudioEngine() {
	stop();
}

void aram::service::JackAdaptedAudioEngine::start() {
}

void aram::service::JackAdaptedAudioEngine::stop() {
	if (jackClient != nullptr) {
		jack_client_close(jackClient);
		jackClient = nullptr;
	}
}




/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Silence adapted audio engine
 */
aram::service::SilenceAdaptedAudioEngine::SilenceAdaptedAudioEngine() :
				mainTurboThread(&SilenceAdaptedAudioEngine::mainTurbo, this), 
				running(false), frameCount_(0) {
	cout << "Hello?" << endl;
	connectFrameReadyObserver(sigc::mem_fun(this, &SilenceAdaptedAudioEngine::onFrameReady));
}

aram::service::SilenceAdaptedAudioEngine::~SilenceAdaptedAudioEngine() {
	cout << "total destruction" << endl;
	stop();
	mainTurboThread.join();
}

void aram::service::SilenceAdaptedAudioEngine::start() {
	frameCount_ = 0;
	running = true;
}

void aram::service::SilenceAdaptedAudioEngine::stop() {
	running = false;
	cout << "frame count = " << frameCount_ << endl;
}

void aram::service::SilenceAdaptedAudioEngine::mainTurbo() {
	while (true) {
		if (running) {
			emitFrameReady(512); //emit
		}
		this_thread::sleep_for(chrono::milliseconds(50));
	}
}

void aram::service::SilenceAdaptedAudioEngine::onFrameReady(unsigned frameCount) {
	frameCount_ += frameCount;
}
