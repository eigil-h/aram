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

#include "jackclient.h"
#include <iostream>

static int jack_callback_process(jack_nframes_t nFrames, void* arg) {
	aram::service::JackClient* jackClient = static_cast<aram::service::JackClient*> (arg);
	aram::model::Samples leftIn = reinterpret_cast<aram::model::Samples> (jack_port_get_buffer(
					jackClient->getJackPort(aram::model::CHANNEL_LEFT, aram::model::DIRECTION_IN),
					nFrames));
	aram::model::Samples rightIn = reinterpret_cast<aram::model::Samples> (jack_port_get_buffer(
					jackClient->getJackPort(aram::model::CHANNEL_RIGHT, aram::model::DIRECTION_IN),
					nFrames));
	aram::model::Samples leftOut = reinterpret_cast<aram::model::Samples> (jack_port_get_buffer(
					jackClient->getJackPort(aram::model::CHANNEL_LEFT, aram::model::DIRECTION_OUT),
					nFrames));
	aram::model::Samples rightOut = reinterpret_cast<aram::model::Samples> (jack_port_get_buffer(
					jackClient->getJackPort(aram::model::CHANNEL_RIGHT, aram::model::DIRECTION_OUT),
					nFrames));

	for (jack_nframes_t i; i < nFrames; i++) {
		*leftOut++ = *leftIn++;
		*rightOut++ = *rightIn++;
	}
	return 0;
}

static int jack_callback_xrun(void* arg) {
	aram::service::JackClient* jackClient = static_cast<aram::service::JackClient*> (arg);
	std::cout << "Xrun, oh no" << std::endl;
}

static uint32_t sampleRate_ = 0;

static int jack_callback_srate(jack_nframes_t nFrames, void* arg) {
	std::cout << "WARZAW@JACK - the sample rate is now " << nFrames << " per second" << std::endl;
	sampleRate_ = nFrames;
	return 0;
}

static void jack_callback_error(const char* description) {
	std::cout << "WARZAW@JACK ERROR - " << description << std::endl;
}

static void jack_callback_shutdown(void* arg) {
	std::cout << "WARZAW@JACK - Good bye to Jack Server.." << std::endl;
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * JackClient
 */

aram::service::JackClient::JackClient() throw (exception) {
	jack_set_error_function(jack_callback_error);

	jack_status_t status;
	jackClient = jack_client_open("aram", JackNullOption, &status);
	if (jackClient == nullptr) {
		cout << "Jack server is not running." << endl;
		throw exception();
	}
	jack_set_process_callback(jackClient, jack_callback_process, this);
	jack_set_xrun_callback(jackClient, jack_callback_xrun, this);
	jack_set_sample_rate_callback(jackClient, jack_callback_srate, NULL);
	jack_on_shutdown(jackClient, jack_callback_shutdown, NULL);

	stereoPort[DIRECTION_IN].registerPort(jackClient, DIRECTION_IN);
	stereoPort[DIRECTION_OUT].registerPort(jackClient, DIRECTION_OUT);

	int errorCode = jack_activate(jackClient);
	if (errorCode != 0) {
		cout << "Jack activation failed with error " << errorCode << endl;
		throw exception();
	}

	stereoPort[DIRECTION_IN].connect(jackClient, DIRECTION_IN);
	stereoPort[DIRECTION_OUT].connect(jackClient, DIRECTION_OUT);
}

aram::service::JackClient::~JackClient() {
	jack_client_close(jackClient);
}

unsigned aram::service::JackClient::sampleRate() {
	return sampleRate_;
}

jack_port_t* aram::service::JackClient::getJackPort(StereoChannel channel, Direction d) {
	return stereoPort[d].getJackPort(channel);
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * JackStereoPort
 */

void aram::service::JackStereoPort::registerPort(jack_client_t* jackClient, Direction direction) throw (exception) {
	ports[CHANNEL_LEFT] = jack_port_register(jackClient,
					(std::string("aram") + (direction == DIRECTION_IN ? "-in" : "-out") + "-left").c_str(),
					JACK_DEFAULT_AUDIO_TYPE, direction == DIRECTION_IN ? JackPortIsInput : JackPortIsOutput, 0L);
	ports[CHANNEL_RIGHT] = jack_port_register(jackClient,
					(std::string("aram") + (direction == DIRECTION_IN ? "-in" : "-out") + "-right").c_str(),
					JACK_DEFAULT_AUDIO_TYPE, direction == DIRECTION_IN ? JackPortIsInput : JackPortIsOutput, 0L);
	if (ports[CHANNEL_LEFT] == nullptr || ports[CHANNEL_RIGHT] == nullptr) {
		cout << "Jack ports not available" << endl;
		throw exception();
	}
}

void aram::service::JackStereoPort::connect(jack_client_t* jack_client, Direction direction) throw (exception) {
	JackGetPorts jackGetPorts(jack_client, JackPortIsPhysical | (direction == DIRECTION_IN ? JackPortIsOutput : JackPortIsInput));
	if (jackGetPorts.isPortSize(2)) {
		for (int i = 0; i < 2; i++) {
			int result;
			if (direction == DIRECTION_IN) {
				result = jack_connect(jack_client, jackGetPorts.getPort(i), jack_port_name(ports[i]));
			} else {
				result = jack_connect(jack_client, jack_port_name(ports[i]), jackGetPorts.getPort(i));
			}
			if (result != 0) {
				cout << "can't connect ports" << endl;
				throw std::exception();
			}
		}
	}
}

jack_port_t* aram::service::JackStereoPort::getJackPort(StereoChannel channel) {
	return ports[channel];
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * JackGetPorts
 */

aram::service::JackGetPorts::JackGetPorts(jack_client_t* jackClient, unsigned long flags) : portsSize(0) {
	ports = ::jack_get_ports(jackClient, nullptr, nullptr, flags);
	if (ports == nullptr) {
		cout << "Can't connect to physical ports" << endl;
		throw exception();
	}
	while (ports[portsSize] != nullptr) {
		portsSize++;
	}
}

aram::service::JackGetPorts::~JackGetPorts() {
	jack_free(ports);
}

bool aram::service::JackGetPorts::isPortSize(unsigned int size) {
	return portsSize == size;
}

const char* aram::service::JackGetPorts::getPort(unsigned int portNumber) {
	if (portNumber >= portsSize) {
		cout << portNumber << "-> No such port! Number of ports are " << portsSize << endl;
		throw std::exception();
	}
	return ports[portNumber];
}
