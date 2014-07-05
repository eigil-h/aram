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

#ifndef WARZAW_MENUDIALOG_H
#define WARZAW_MENUDIALOG_H

#include <memory>
#include <gtkmm.h>

using namespace std;

namespace aram {
	/** 
	 * Graphical user interface name space
	 */
	namespace gui {

		class EventBroadcasterSingleton {
		public:
			static EventBroadcasterSingleton& getInstance();

			sigc::signal<void> projectChangedSignal;
			sigc::signal<void> audioclipChangedSignal;

			void projectChanged();
			void audioclipChanged();

		private:
			EventBroadcasterSingleton();
			// noncopyable
			EventBroadcasterSingleton(const EventBroadcasterSingleton&);
			void operator=(const EventBroadcasterSingleton&);
		};

		class TopSection;
		class MainSection;

		class MenuDialog : public Gtk::Dialog {
			Gtk::VBox sections;
			unique_ptr<TopSection> topSection;
			unique_ptr<MainSection> mainSection;
		public:
			MenuDialog();

			void refresh();
		};

		class TopSection : public Gtk::HBox {
			MenuDialog* menuDialog;

			Gtk::VBox buttons;
			Gtk::Button newProject;
			Gtk::Button newAudioclip;
			Gtk::Frame aboutFrame;
			Gtk::Label aboutLabel;

			void onNewProject();
			void onNewAudioclip();

		public:
			TopSection(MenuDialog* md);
		};

		class BaseColumns : public Gtk::TreeModelColumnRecord {
		public:
			BaseColumns();

			Gtk::TreeModelColumn<Glib::ustring> id;
			Gtk::TreeModelColumn<Glib::ustring> name;
			Gtk::TreeModelColumn<Glib::DateTime> lastOpened;
			Gtk::TreeModelColumn<unsigned> samplingRate;
			Gtk::TreeModelColumn<unsigned> length; //in seconds
		};

		class ProjectColumns : public BaseColumns {
		public:
			ProjectColumns();
		};

		class AudioclipColumns : public BaseColumns {
		public:
			AudioclipColumns();
		};

		class BaseListView : public Gtk::TreeView {
			bool renameCanceled;

		protected:
			void onSelectedChanged();
			void onRenameStarted(Gtk::CellEditable* editable, const Glib::ustring& path);
			void onRenameCanceled();
			void onRenameEnded();

			virtual void refresh() = 0;
			virtual void onSelected(const Gtk::TreeModel::Row& row) = 0;
			virtual void onRenamed(const Gtk::TreeModel::Row& row) = 0;

		public:
			BaseListView();

			void onReload();
		};

		class ProjectListView : public BaseListView {
			ProjectColumns columns;
			Glib::RefPtr<Gtk::ListStore> listStore;

			void onSelected(const Gtk::TreeModel::Row& row);
			void onRenamed(const Gtk::TreeModel::Row& row);

		public:
			ProjectListView();

			void refresh();
		};

		class AudioclipListView : public BaseListView {
			AudioclipColumns columns;
			Glib::RefPtr<Gtk::ListStore> listStore;

			void onSelected(const Gtk::TreeModel::Row& row);
			void onRenamed(const Gtk::TreeModel::Row& row);

		public:
			AudioclipListView();

			void refresh();
		};

		class MainSection : public Gtk::HBox {
			MenuDialog* menuDialog;
			ProjectListView projectListView;
			AudioclipListView audioclipListView;

		public:
			MainSection(MenuDialog* md);

			void refresh();
		};
	}
}

#endif
