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
#include "../easylogging++.h"

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
				backBufferRunning(true),
				pos_(0) {
}

aram::service::AudioEngine::~AudioEngine() {
	backBufferRunning = false;
	backBufferThread.join();
}

void aram::service::AudioEngine::onPlaybackPositionChange(aram::service::PlaybackPos pos) {
	pos_ = pos;
	if(!playback) {
		for_each(channels.begin(), channels.end(), [=](const pair<string, unique_ptr<ChannelPlayer>>& ch){
			ch.second->setPosition(pos);
		});
	}
}

void aram::service::AudioEngine::onSampleRateChange(unsigned sampleRate) {
	LOG(DEBUG) << "Sample rate change from " << sampleRate_ << " to " << sampleRate;
	sampleRate_ = sampleRate;
}

void aram::service::AudioEngine::backBufferTurbo() {
	while(backBufferRunning) {
		this_thread::sleep_for(chrono::milliseconds(1000));

		if (playback) {
			if (recorder.get() != nullptr) {
				recorder->swapAndStore();
			}
		}
		
		for_each(channels.begin(), channels.end(), [&](pair<string, unique_ptr<ChannelPlayer>>& channel) {
			channel.second->loadBackBuffers();
		});
	}
}

void aram::service::AudioEngine::addChannel(const string& channel) {
	channels.push_front(make_pair(channel, unique_ptr<ChannelPlayer>(new ChannelPlayer(channel))));
}

void aram::service::AudioEngine::removeChannel(const string& channel) {
}

void aram::service::AudioEngine::armChannel(const string& channel) {
	recorder.reset(new Recorder(channel));
}

void aram::service::AudioEngine::disarmChannel() {
	recorder.reset();
}

unsigned aram::service::AudioEngine::sampleRate() {
	return sampleRate_;
}


/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * AudioEngineSignals
 */
aram::service::AudioEngineSignals& aram::service::AudioEngineSignals::getInstance() {
	static AudioEngineSignals instance;
	return instance;
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Recorder
 */
aram::service::Recorder::Recorder(const string& audioclipId_) :
				audioclipId(audioclipId_),
				recordingBufferLeft(491520),
				recordingBufferRight(491520),
				recordingStreamLeft((System::getHomePath() + "/.aram/" + audioclipId_ + "-l").c_str(),
								ios_base::binary | ios_base::trunc),
				recordingStreamRight((System::getHomePath() + "/.aram/" + audioclipId_ + "-r").c_str(),
								ios_base::binary | ios_base::trunc) {

	LOG(INFO) << "Recorder for " << audioclipId << " created";
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
 * ChannelPlayer
 * 
 * note: so far there's only one stream/audioclip per channel
 */
aram::service::ChannelPlayer::ChannelPlayer(const string& channel_) :
				channel(channel_) {

	ifstream ifstr_l((System::getHomePath() + "/.aram/" + channel_ + "-l").c_str(), ios_base::binary);
	forward_list<istream*> istrList_l = {&ifstr_l};
	playbackSD[STEREO_LEFT] = make_pair(
					unique_ptr<LoadAndReadBuffer>(new LoadAndReadBuffer(491520)), istrList_l);


	ifstream ifstr_r((System::getHomePath() + "/.aram/" + channel_ + "-r").c_str(), ios_base::binary);
	forward_list<istream*> istrList_r = {&ifstr_r};
	playbackSD[STEREO_RIGHT] = make_pair(
					unique_ptr<LoadAndReadBuffer>(new LoadAndReadBuffer(491520)), istrList_r);
}

bool aram::service::ChannelPlayer::playback(Samples left, Samples right, unsigned count) {
	playbackSD[STEREO_LEFT].first->readFrontBuffer(left, count);
	playbackSD[STEREO_RIGHT].first->readFrontBuffer(right, count);
}

void aram::service::ChannelPlayer::loadBackBuffers() {
	playbackSD[STEREO_LEFT].first->loadBackBuffer(playbackSD[STEREO_LEFT].second);
	playbackSD[STEREO_RIGHT].first->loadBackBuffer(playbackSD[STEREO_RIGHT].second);
}

void aram::service::ChannelPlayer::setPosition(PlaybackPos pos) {
	//from top of the istream list, calculate the way down to stream
	
	//but !! something is not right here. We can't have all streams for channel around here!
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Jack adapted audio engine
 */
static int onFrameReadyJackFun(uint32_t frameCount, void* ignore) {
	aram::service::AudioEngineSignals& signals = aram::service::AudioEngineSignals::getInstance();
	signals.frameReadySignal(frameCount);
	return JACK_CALLBACK_SUCCESS;
}

static int onXRunJackFun(void* ignore) {
	aram::service::AudioEngineSignals& signals = aram::service::AudioEngineSignals::getInstance();
	signals.xRunSignal();
	return JACK_CALLBACK_SUCCESS;
}

static int onSampleRateChangeJackFun(unsigned sampleRate, void* ignore) {
	aram::service::AudioEngineSignals& signals = aram::service::AudioEngineSignals::getInstance();
	signals.sampleRateChangeSignal(sampleRate);
	return JACK_CALLBACK_SUCCESS;
}

static void onShutdownJackFun(void* ignore) {
	aram::service::AudioEngineSignals& signals = aram::service::AudioEngineSignals::getInstance();
	signals.shutdownSignal();
}

static void onErrorJackFun(const char* msg) {
	aram::service::AudioEngineSignals& signals = aram::service::AudioEngineSignals::getInstance();
	signals.errorSignal(msg);
}

aram::service::JackAdaptedAudioEngine::JackAdaptedAudioEngine() {
	jack_set_error_function(onErrorJackFun);

	jack_status_t status;
	jackClient = jack_client_open("aram", JackNullOption, &status);
	if (jackClient == nullptr) {
		throw runtime_error("Jack server is not running.");
	}

	sampleRate_ = jack_get_sample_rate(jackClient);

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
	
	AudioEngineSignals& signals = AudioEngineSignals::getInstance();

	signals.frameReadySignal.connect(sigc::mem_fun(this, &JackAdaptedAudioEngine::onFrameReady));
	signals.playbackPosChangeSignal.connect(sigc::mem_fun(this, &JackAdaptedAudioEngine::onPlaybackPositionChange));
	signals.sampleRateChangeSignal.connect(sigc::mem_fun(this, &JackAdaptedAudioEngine::onSampleRateChange));
}


aram::service::JackAdaptedAudioEngine::~JackAdaptedAudioEngine() {
	if (jackClient != nullptr) {
		jack_client_close(jackClient);
	}
}

void aram::service::JackAdaptedAudioEngine::onFrameReady(unsigned frameCount) {
	Samples leftIn = reinterpret_cast<Samples> (jack_port_get_buffer(
					physicalInputPort.ports[STEREO_LEFT], frameCount));
	Samples rightIn = reinterpret_cast<Samples> (jack_port_get_buffer(
					physicalInputPort.ports[STEREO_RIGHT], frameCount));
	Samples leftOut = reinterpret_cast<Samples> (jack_port_get_buffer(
					physicalOutputPort.ports[STEREO_LEFT], frameCount));
	Samples rightOut = reinterpret_cast<Samples> (jack_port_get_buffer(
					physicalOutputPort.ports[STEREO_RIGHT], frameCount));

	//unconditionally copy input buffer to output buffer for immediate playback
	::memcpy(leftOut, leftIn, sizeof (Sample) * frameCount);
	::memcpy(rightOut, rightIn, sizeof (Sample) * frameCount);

	if (playback) {
		for_each(channels.begin(), channels.end(), [&](pair<string, unique_ptr<ChannelPlayer>>& channel) {
			Samples leftPortOut = reinterpret_cast<Samples> (jack_port_get_buffer(
							channelPorts[channel.first].ports[STEREO_LEFT], frameCount));
			Samples rightPortOut = reinterpret_cast<Samples> (jack_port_get_buffer(
							channelPorts[channel.first].ports[STEREO_RIGHT], frameCount));
			
			channel.second->playback(leftPortOut, rightPortOut, frameCount);
		});

		AudioEngineSignals& signals = AudioEngineSignals::getInstance();

		signals.playbackPosChangeSignal(pos_ + frameCount);

		if (recorder.get() != nullptr) {
			if(!recorder->record(leftIn, rightIn, frameCount)) {
				signals.xRunSignal(); //todo - maybe a parameter to explain it's not really an xrun...?
			}
		}
	}
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Silence adapted audio engine
 */
aram::service::SilenceAdaptedAudioEngine::SilenceAdaptedAudioEngine() :
				mainTurboThread(&SilenceAdaptedAudioEngine::mainTurbo, this),
				running(true), frameCountRecording(0),
				frameCountTotal(0) {
	LOG(INFO) << "Constructing the Silence Adapted Audio Engine";

	sampleRate_ = 1000L / 50;

	AudioEngineSignals& signals = AudioEngineSignals::getInstance();

	signals.frameReadySignal.connect(sigc::mem_fun(this, &SilenceAdaptedAudioEngine::onFrameReady));
	signals.playbackPosChangeSignal.connect(sigc::mem_fun(this, &SilenceAdaptedAudioEngine::onPlaybackPositionChange));
	signals.sampleRateChangeSignal.connect(sigc::mem_fun(this, &SilenceAdaptedAudioEngine::onSampleRateChange));
}

aram::service::SilenceAdaptedAudioEngine::~SilenceAdaptedAudioEngine() {
	LOG(INFO) << "Destroying the Silence Adapted Audio Engine";

	running = false;
	this_thread::sleep_for(chrono::milliseconds(100));

	LOG(INFO) << "frame count played back = " << pos_;
	LOG(INFO) << "frame count recorded = " << frameCountRecording;
	LOG(INFO) << "frame count total = " << frameCountTotal;

	mainTurboThread.join();
}

void aram::service::SilenceAdaptedAudioEngine::mainTurbo() {
	while (running) {
		AudioEngineSignals::getInstance().frameReadySignal(1);
		this_thread::sleep_for(chrono::milliseconds(50));
	}
}

void aram::service::SilenceAdaptedAudioEngine::onFrameReady(unsigned frameCount) {
	frameCountTotal += frameCount;
	if (playback) {
		AudioEngineSignals::getInstance().playbackPosChangeSignal(pos_ + frameCount);

		if (recorder.get() != nullptr) {
			frameCountRecording += frameCount;
		}
	}
}
