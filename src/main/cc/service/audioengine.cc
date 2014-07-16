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

void aram::service::AudioEngine::addFrameReadyObserver(void_int_cb c) {
	onFrameReadyObserver.emplace(c);
}

void aram::service::AudioEngine::addXRunObserver(void_void_cb c) {
	onXRunObserver.emplace(c);
}

void aram::service::AudioEngine::addSampleRateChangeObserver(void_int_cb c) {
	onSampleRateChangeObserver.emplace(c);
}

void aram::service::AudioEngine::addShutdownObserver(void_void_cb c) {
	onShutdownObserver.emplace(c);
}

void aram::service::AudioEngine::addErrorObserver(void_constcharstar_cb c) {
	onErrorObserver.emplace(c);
}

int aram::service::AudioEngine::onFrameReady(unsigned frameCount) {
	for (void_int_cb cb : onFrameReadyObserver) {
		cb(frameCount);
	}
}

int aram::service::AudioEngine::onXRun() {
	for (void_void_cb cb : onXRunObserver) {
		cb();
	}
}

int aram::service::AudioEngine::onSampleRateChange(unsigned sampleRate) {
	for (void_int_cb cb : onSampleRateChangeObserver) {
		cb(sampleRate);
	}
}

void aram::service::AudioEngine::onShutdown() {
	for (void_void_cb cb : onShutdownObserver) {
		cb();
	}
}

void aram::service::AudioEngine::onError(const char* msg) {
	for (void_constcharstar_cb cb : onErrorObserver) {
		cb(msg);
	}
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Jack adapted audio engine
 */
aram::service::JackAdaptedAudioEngine::JackAdaptedAudioEngine() {
}

aram::service::JackAdaptedAudioEngine::~JackAdaptedAudioEngine() {
	stop();
}

static int onFrameReadyJackFun(uint32_t frameCount, void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.onFrameReady(frameCount);
}

static int onXRunJackFun(void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.onXRun();
}

static int onSampleRateChangeJackFun(unsigned sampleRate, void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.onSampleRateChange(sampleRate);
}

static void onShutdownJackFun(void* ignore) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.onShutdown();
}

static void onErrorJackFun(const char* msg) {
	aram::service::AudioEngine& audioEngine = aram::service::AudioEngineFactory::audioEngine();
	audioEngine.onError(msg);
}

void aram::service::JackAdaptedAudioEngine::init() {
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
void aram::service::SilenceAdaptedAudioEngine::mainTurbo() {
	while (running) {
		this_thread::sleep_for(chrono::milliseconds(50));
	}
	//	cout << "total frames were " << project->frames() << endl;
}

aram::service::SilenceAdaptedAudioEngine::SilenceAdaptedAudioEngine() :
mainTurboThread(&SilenceAdaptedAudioEngine::mainTurbo, this), running(true) {
	//	sampleRate_ = 100;
	//Load from database the current project
	//Application app;
	//app.load();
	//	project = app.project();
	cout << "Silence audio engine has started." << endl;
}

aram::service::SilenceAdaptedAudioEngine::~SilenceAdaptedAudioEngine() {
	running = false;
	mainTurboThread.join();

	cout << "Silence audio engine has been shut down." << endl;
}

void aram::service::SilenceAdaptedAudioEngine::init() {
}

void aram::service::SilenceAdaptedAudioEngine::start() {
}

void aram::service::SilenceAdaptedAudioEngine::stop() {
}
