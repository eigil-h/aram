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

#ifndef WARSAW_PROJECT_H
#define WARSAW_PROJECT_H

#include <string>
#include <memory>
#include <set>
#include <odb/core.hxx>
#include "audioclip.h"

using namespace std;

namespace warsaw {
	namespace model {

		#pragma db object pointer(std::shared_ptr)
		class Project {
			friend class odb::access;

			#pragma db id
			string id_;
			string name_;
			#pragma db value_not_null
			set<shared_ptr<Audioclip>, Audioclip::Less> audioclips_;
			#pragma db not_null
			shared_ptr<Audioclip> armed_;
			unsigned frames_;
			unsigned sampleRate_;

		public:
			Project();
			Project(const string& name, const shared_ptr<Audioclip>& ac);
			~Project();

			bool operator==(const Project& other) const;
			bool operator<(const Project& other) const;

			const string& id() const;
			const string& name() const;
			const set<shared_ptr<Audioclip>, Audioclip::Less>& audioclips() const;
			const Audioclip& armed() const;
			shared_ptr<Audioclip> findAudioclip(const string& id) const;
			const unsigned& frames() const;
			unsigned length() const;
			const unsigned& sampleRate() const;
			void addAudioclip(shared_ptr<Audioclip> ac);
			void arm(const shared_ptr<Audioclip>& ac);

			/* "(For PCM, A-law and μ-law data,) a frame is all data that belongs to one sampling interval.
			 * This means that the frame rate is the same as the sample rate."
			 * http://www.jsresources.org/faq_audio.html#frame_rate
			 *
			 * The audio engine calls this method after processing nFrames number of frames.
			 * This way the project can update its counter on playback.
			 */
			void audioEngineProcessedFrames(unsigned nFrames);

			void rename(const string& newName);

			static set<Project> findAll();
			static void createNew();
			static shared_ptr<Project> retrieveCurrent();
			static shared_ptr<Project> retrieveById(const string& id);
		};

		/**
		 * Application configuration. Most importantly it holds the current active project.
		 */
		#pragma db object
		class Application {

			friend class odb::access;

			#pragma db id
			string name_;

			#pragma db not_null
			shared_ptr<Project> project_;

		public:
			Application();
			Application(const string& name, const shared_ptr<Project>& project);
			const string& name() const;
			const shared_ptr<Project>& project() const;
			void load();
			void selectProject(const string& id);
		};

		#pragma db view object(Application)

		struct ApplicationStats {
			#pragma db column("count(" + Application::name_ + ")")
			std::size_t count;
		};
	}
}


#endif
