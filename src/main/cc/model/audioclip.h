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

#ifndef ARAM_AUDIOCLIP_H
#define ARAM_AUDIOCLIP_H

#include <string>
#include <memory>
#include <map>
#include <cstdint>
#include <odb/core.hxx>
#include "../service/types.h"

using namespace std;

namespace aram {
	using namespace service;

	namespace model {

		#pragma db object pointer(std::shared_ptr)
		class Audioclip {
			Audioclip();

			friend class odb::access;

			#pragma db id
			string id_;
			string name_;
			unsigned sampleRate_;

		public:
			Audioclip(const string& name);
			~Audioclip();
			
			bool operator==(const Audioclip& ac) const;
			bool operator<(const Audioclip& other) const;

			const string& id() const;
			const string& name() const;
			void name(const string& name);
			PlaybackPos length() const;
			const unsigned& sampleRate() const;

			void rename(const string& newName);

			static void createNew();
			static shared_ptr<Audioclip> retrieveById(const string& id);

			struct Less {
				bool operator() (const shared_ptr<Audioclip>& a, const shared_ptr<Audioclip>& b);
			};
		};


		#pragma db object pointer(std::shared_ptr)
		class Channel {
			Channel();

			friend class odb::access;

			#pragma db id
			string id_;
			string name_;
			map<PlaybackPos, shared_ptr<Audioclip>> audioclips_;

		public:
			Channel(const string& name, const shared_ptr<Audioclip>& ac);
			~Channel();

			const string& id() const;
			const string& name() const;
			PlaybackPos length() const;

			void addAudioclip(const shared_ptr<Audioclip>& ac, PlaybackPos position);
			shared_ptr<Audioclip> firstAudioclip();
		};
	}
}

#endif

