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

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * AudioEngineFactory
 */
static bool isSilence(int argc, char** argv) {
	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "-silence") == 0) {
			return true;
		}
	}
	return false;
}

unique_ptr<aram::service::AudioEngine>
aram::service::AudioEngineFactory::assemble(int argc, char** argv) {
	AudioEngine* as;
	if (isSilence(argc, argv)) {
		as = new SilenceAdaptedAudioEngine();
	} else {
//		as = new JackClient();
		as = new SilenceAdaptedAudioEngine();
	}
	unique_ptr<AudioEngine> asp(as);
	return asp;
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * AudioEngine
 */
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

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Silence
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
	Application app;
	app.load();
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
