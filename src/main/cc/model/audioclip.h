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

#ifndef WARSAW_AUDIOCLIP_H
#define WARSAW_AUDIOCLIP_H

#include <string>
#include <memory>
#include <odb/core.hxx>

using namespace std;

namespace warsaw {
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
			unsigned length() const;
			const unsigned& sampleRate() const;

			void rename(const string& newName);

			static void createNew();
			static shared_ptr<Audioclip> retrieveById(const string& id);

			struct Less {
				bool operator() (const shared_ptr<Audioclip>& a, const shared_ptr<Audioclip>& b);
			};
		};
	}
}

#endif

