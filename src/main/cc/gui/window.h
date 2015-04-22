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

#ifndef WARZAW_WINDOW_H
#define WARZAW_WINDOW_H

#include <gtkmm.h>

using namespace std;

namespace aram {
	/** 
	 * Graphical user interface name space
	 */
	namespace gui {

		/** Combobox where to select active channel for receiving input for recording.
		 */
		class ReceivingChannelBox : public Gtk::VBox {

			struct Model : public Gtk::TreeModel::ColumnRecord {
				Model();
				Gtk::TreeModelColumn<Glib::ustring> channelId;
				Gtk::TreeModelColumn<Glib::ustring> channelName;
			};
		public:
			ReceivingChannelBox();
			~ReceivingChannelBox();

			Glib::ustring channelId() const;

		private:
			Gtk::TreeModel::Row activeRow;
			sigc::connection focusGainedConnection;
			sigc::connection focusLostConnection;
			Gtk::ComboBox combo;
			Model comboModel;
			Glib::RefPtr<Gtk::ListStore> comboModelRef;

			Gtk::HBox buttonBox;
			Gtk::Button addButton;
			Gtk::Button removeButton;

			void onSelected();
			void onActivated();
			bool onFocusGained(GdkEventFocus* event);
			bool onFocusLost(GdkEventFocus* event);
			void onAddChannel();
			void onRemoveChannel();
		};

		/** 
		 * The most used/useful commands at your fingertips.
		 */
		class CommandContainer : public Gtk::HBox {
		public:
			CommandContainer();

		private:
			Gtk::ToggleButton playButton;
			Gtk::ToggleButton recordButton;
			ReceivingChannelBox channelBox;

			void onPlayButtonClicked();
			void onRecordButtonClicked();
		};

		class ApplicationMenu : public Gtk::HBox {
		public:
			ApplicationMenu();

		private:
			Gtk::Button undo;
			Gtk::Button prefs;
			Gtk::Button help;

			void onUndoClicked();
		};

		class ProjectMenu : public Gtk::HBox {
		public:
			ProjectMenu();

		private:
			Gtk::Button create;
			Gtk::Button open;
			Gtk::Button edit;
			Gtk::Label stats;
		};

		class AudioclipMenu : public Gtk::HBox {
		public:
			AudioclipMenu();

		private:
			Gtk::Button create;
			Gtk::Button open;
			Gtk::Button edit;
			Gtk::Label stats;
		};

		class Navigator : public Gtk::HBox {
		public:
			Navigator();
		private:
			Gtk::Button start;
			Gtk::Button rewind;
			Gtk::Button backpedal;
			Gtk::Button forward;
			Gtk::Button fastforward;
			Gtk::Button end;
		};

		class AudioclipView : public Gtk::Grid {
		public:
		private:	
			Gtk::ToggleButton record;
			Gtk::Button mark;
			Gtk::Button unmark;
			Gtk::Button markstatus;
			Gtk::Label counter;
			Navigator navigator;
			Gtk::Button trimFront;
			Gtk::Button trimBack;
			Gtk::Button loopSolo;
			Gtk::Button assign;
			Gtk::Button unassign;
		};
		
		class ProjectView : public Gtk::Grid {
		public:
		private:	
		};

		/**
		 * The main window
		 */
		class Window : public Gtk::Window {
			Gtk::VBox frames;
			Gtk::Frame menuFrame;
			Gtk::VBox menuSections;
			Gtk::HBox menuSection1;
			Gtk::HBox menuSection2;
			ApplicationMenu applicationMenu;
			ProjectMenu projectMenu;
			AudioclipMenu audioclipMenu;
			Gtk::Frame bodyFrame;
			Gtk::HBox bodySections;
			AudioclipView audioclipView;
			ProjectView projectView;

		public:
			Window();
		};
	}
}
#endif
