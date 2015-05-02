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

		class ModelDialog : public Gtk::Dialog {
		public:
			ModelDialog(const string& title);

		protected:
			void onActivateResponseOK();
		};

		/** Let user select a project or audioclip.
		 */
		class OpenDialog : public ModelDialog {
		public:
			OpenDialog(const string& title);
			string selectedId();

		protected:
			struct Model : public Gtk::TreeModel::ColumnRecord {
				Model();
				Gtk::TreeModelColumn<Glib::ustring> id;
				Gtk::TreeModelColumn<Glib::ustring> name;
			};

			Gtk::TreeView list;
			Model listModel;
			Glib::RefPtr<Gtk::ListStore> listModelRef;
		};

		class OpenProjectDialog : public OpenDialog {
		public:
			OpenProjectDialog();
		};

		class OpenAudioclipDialog : public OpenDialog {
		public:
			OpenAudioclipDialog();
		};

		/** Let user edit a project or audioclip. Eg rename.
		 */
		class EditDialog : public ModelDialog {
		public:
			EditDialog(const string& title, const string& name);
			string name() const;

		private:
			Gtk::HBox renameBox;
			Gtk::Label renameLabel;
			Gtk::Entry renameEntry;
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

			void onCreate();
			void onOpen();
			void onEdit();
			void onStatsChanged();
		};

		class AudioclipMenu : public Gtk::HBox {
		public:
			AudioclipMenu();

		private:
			Gtk::Button create;
			Gtk::Button open;
			Gtk::Button edit;
			Gtk::Label stats;

			void onCreate();
			void onOpen();
			void onEdit();
			void onStatsChanged();
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
			AudioclipView();
		private:
			Gtk::ToggleButton record;
			Gtk::Button mark;
			Gtk::Button unmark;
			Gtk::ToggleButton play;
			Gtk::Label counter;
			Navigator navigator;
			Gtk::Button trimFront;
			Gtk::Button trimBack;
			Gtk::Button assign;
			Gtk::Button unassign;
			
			void onRecordButtonClicked();
			void onMarkButtonPressed();
			void onMarkButtonReleased();
			void onUnmarkButtonClicked();
			void onPlayButtonClicked();
			void onTrimFrontButtonClicked();
			void onTrimBackButtonClicked();
			void onAssignButtonClicked();
			void onUnassignButtonClicked();
		};

		class ChannelView : public Gtk::TreeView {
		public:
		private:
		};

		class ProjectView : public Gtk::Grid {
		public:
			ProjectView();
		private:
			Gtk::Label counter;
			Navigator navigator;
			Gtk::ToggleButton play;
			Gtk::ToggleButton loop;
			Gtk::Button addChannel;
			ChannelView channelView;
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

		class GuiSignal {
		public:
			static GuiSignal& getInstance();

			sigc::signal<void> projectEdited;
			sigc::signal<void> anotherProjectSelected;
			sigc::signal<void> audioclipEdited;
			sigc::signal<void> anotherAudioclipSelected;
		private:
		};
	}
}
#endif
