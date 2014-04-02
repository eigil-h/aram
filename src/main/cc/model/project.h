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
			set<shared_ptr<AudioClip>> audioClips_;
			#pragma db not_null
			shared_ptr<AudioClip> armed_;
			unsigned frames_;

		public:
			Project();
			Project(const string& name, const shared_ptr<AudioClip>& ac);
			~Project();

			const string& name() const;
			const unsigned& frames() const;

			/* "(For PCM, A-law and μ-law data,) a frame is all data that belongs to one sampling interval.
			 * This means that the frame rate is the same as the sample rate."
			 * http://www.jsresources.org/faq_audio.html#frame_rate
			 *
			 * The audio engine calls this method after processing nFrames number of frames.
			 * This way the project can update its counter on playback.
			 */
			void audioEngineProcessedFrames(unsigned nFrames);
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
		};

		#pragma db view object(Application)

		struct ApplicationStats {
			#pragma db column("count(" + Application::name_ + ")")
			std::size_t count;
		};
	}
}


#endif
