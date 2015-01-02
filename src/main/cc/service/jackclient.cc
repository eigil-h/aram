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

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * JackStereoPort
 */

void aram::service::JackStereoPort::registerPort(jack_client_t* jackClient, 
				Direction direction, const string& name) {
	ports[CHANNEL_LEFT] = jack_port_register(jackClient,
					(std::string("aram-") + name + "-left").c_str(), JACK_DEFAULT_AUDIO_TYPE, 
					direction == DIRECTION_INPUT ? JackPortIsInput : JackPortIsOutput, 0L);

	ports[CHANNEL_RIGHT] = jack_port_register(jackClient,
					(std::string("aram-") + name + "-right").c_str(), JACK_DEFAULT_AUDIO_TYPE, 
					direction == DIRECTION_INPUT ? JackPortIsInput : JackPortIsOutput, 0L);

	if (ports[CHANNEL_LEFT] == nullptr || ports[CHANNEL_RIGHT] == nullptr) {
		throw runtime_error("Jack ports not available");
	}
}


void aram::service::JackStereoPort::connectPhysicalPort(jack_client_t* jack_client, Direction direction) {
	JackGetPorts jackGetPorts(jack_client, JackPortIsPhysical | (direction == DIRECTION_INPUT ? JackPortIsOutput : JackPortIsInput));
	if (jackGetPorts.isPortSize(2)) {
		for (int i = 0; i < 2; i++) {
			int result;
			if (direction == DIRECTION_INPUT) {
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


/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * JackGetPorts
 */

aram::service::JackGetPorts::JackGetPorts(jack_client_t* jackClient, unsigned long flags) : portsSize(0) {
	ports = ::jack_get_ports(jackClient, nullptr, nullptr, flags);
	if (ports == nullptr) {
		throw runtime_error("Can't connect to physical ports");
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
		throw runtime_error(portNumber + "-> No such port! Number of ports are " + portsSize);
	}
	return ports[portNumber];
}
