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

#ifndef WARZAW_WINDOW_H
#define WARZAW_WINDOW_H

#include <gtkmm.h>

using namespace std;

namespace aram {
	/** 
	 * Graphical user interface name space
	 */
	namespace gui {

		/** 
		 * The most used/useful commands at your fingertips.
		 */
		class CommandContainer : public Gtk::VBox {
			Gtk::Button menuButton;
			Gtk::ToggleButton playButton;
			Gtk::ToggleButton recordButton;
			Gtk::Button markButton;
			
			void onMenuButtonClicked();
			void onPlayButtonClicked();
			void onRecordButtonClicked();
			void onMarkButtonPressed();
			void onMarkButtonReleased();
			
		public:
			CommandContainer();
		};

		class Monitor : public Gtk::VBox {
		public:
			Monitor();
		};
		
		class ProjectView : public Gtk::DrawingArea {
		public:
			ProjectView();
		};
		
		/** 
		 * Holding project and audioclip views
		 */
		class BodyContainer : public Gtk::VBox {
			Monitor projectMonitor;
			Monitor audioclipMonitor;
			ProjectView projectView;
		public:
			BodyContainer();
		};

		/**
		 * The main window
		 */
		class Window : public Gtk::Window {
			Gtk::HBox topContainer;
			CommandContainer commandContainer;
			BodyContainer bodyContainer;

		public:
			Window();
		};
	}
}

#endif
