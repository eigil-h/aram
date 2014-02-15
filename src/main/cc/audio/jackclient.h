/*
	Warsaw, the audio recorder and music composer
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

#ifndef WARSAW_JACKCLIENT_H
#define WARSAW_JACKCLIENT_H

#include <jack/jack.h>
#include <exception>

#include "types.h"

using namespace std;

namespace warsaw {
	namespace audio {

		/**
		 * Wrapper for two jack_port_t
		 */
		class JackStereoPort {
			jack_port_t* ports[CHANNEL_sizeof];

		public:
			void registerPort(jack_client_t* jackClient, Direction direction) throw (exception);
			void connect(jack_client_t* jackClient, Direction direction) throw (exception);
			jack_port_t* getJackPort(StereoChannel channel);
		};

		/**
		 * Wrapper for jack_client_t
		 */
		class JackClient {
			jack_client_t* jackClient;

		public:
			JackStereoPort stereoPort[DIRECTION_sizeof];

			JackClient() throw (exception);
			virtual ~JackClient();

			unsigned getSampleRate();
			jack_port_t* getJackPort(StereoChannel channel, Direction d);
		};

		/**
		 * Wrapper for jack_get_ports()
		 */
		class JackGetPorts {
			const char** ports;
			unsigned int portsSize;

		public:
			JackGetPorts(jack_client_t* jackClient, unsigned long flags);
			virtual ~JackGetPorts();

			bool isPortSize(unsigned int size);
			const char* getPort(unsigned int i);
		};
	}
}
#endif
