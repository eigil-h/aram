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

		/**
		 * The main window
		 */
		class Window : public Gtk::Window {
			CommandContainer commandContainer;

		public:
			Window();
		};
	}
}

#endif
