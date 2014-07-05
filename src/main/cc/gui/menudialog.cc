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

#include "menudialog.h"
#include <iostream>
#include "../model/project.h"
#include "../model/audioclip.h"

using namespace aram::model;

aram::gui::EventBroadcasterSingleton::EventBroadcasterSingleton() {
}

aram::gui::EventBroadcasterSingleton& aram::gui::EventBroadcasterSingleton::getInstance() {
	static EventBroadcasterSingleton instance;
	return instance;
}

void aram::gui::EventBroadcasterSingleton::projectChanged() {
	projectChangedSignal.emit();
}

void aram::gui::EventBroadcasterSingleton::audioclipChanged() {
	audioclipChangedSignal.emit();
}

aram::gui::MenuDialog::MenuDialog() : Gtk::Dialog("Menu", true),
topSection(new TopSection(this)), mainSection(new MainSection(this)) {

	sections.pack_start(*topSection);
	sections.pack_start(*mainSection);
	get_content_area()->pack_start(sections);
	get_content_area()->set_size_request(900, 768);
	get_content_area()->show_all();
}

void aram::gui::MenuDialog::refresh() {
	mainSection->refresh();
}

aram::gui::TopSection::TopSection(MenuDialog* md) : menuDialog(md),
newProject("New Project"),
newAudioclip("New Audioclip"),
aboutFrame("About"),
aboutLabel("2014 (C) Audio Recorder And Music, aram.ninja, GPL3  license") {

	newProject.signal_clicked().connect(
					sigc::mem_fun(this, &TopSection::onNewProject));

	newAudioclip.signal_clicked().connect(
					sigc::mem_fun(this, &TopSection::onNewAudioclip));

	buttons.pack_start(newProject);
	buttons.pack_start(newAudioclip);
	aboutFrame.add(aboutLabel);

	pack_start(buttons);
	pack_start(aboutFrame);
}

void aram::gui::TopSection::onNewProject() {
	Project::createNew();
	menuDialog->refresh();
}

void aram::gui::TopSection::onNewAudioclip() {
	Audioclip::createNew();
	menuDialog->refresh();
}

aram::gui::MainSection::MainSection(MenuDialog* md) : menuDialog(md) {
	pack_start(projectListView);
	pack_start(audioclipListView);
}

void aram::gui::MainSection::refresh() {
	projectListView.refresh();
	audioclipListView.refresh();
}

aram::gui::BaseColumns::BaseColumns() {
	add(id);
	add(name);
	add(lastOpened);
	add(samplingRate);
	add(length);
}

aram::gui::ProjectColumns::ProjectColumns() : BaseColumns() {
}

aram::gui::AudioclipColumns::AudioclipColumns() : BaseColumns() {
}

aram::gui::BaseListView::BaseListView() : renameCanceled(false) {
}

aram::gui::ProjectListView::ProjectListView() : listStore(Gtk::ListStore::create(columns)) {

	set_model(listStore);

	append_column_editable("Name", columns.name);
	append_column("SampleRate", columns.samplingRate);
	append_column("Length", columns.length);

	refresh();

	get_selection()->signal_changed().connect(
					mem_fun(this, &ProjectListView::onSelectedChanged));

	get_column_cell_renderer(0)->signal_editing_started().connect(
					mem_fun(this, &ProjectListView::onRenameStarted));

	get_column_cell_renderer(0)->signal_editing_canceled().connect(
					mem_fun(this, &ProjectListView::onRenameCanceled));
}

void aram::gui::BaseListView::onSelectedChanged() {
	Gtk::TreeModel::iterator itr = get_selection()->get_selected();
	if (itr) {
		onSelected(*itr);
	}
}

void aram::gui::BaseListView::onRenameStarted(Gtk::CellEditable* editable, const Glib::ustring& path) {
	editable->signal_editing_done().connect(
					mem_fun(this, &BaseListView::onRenameEnded));
}

void aram::gui::BaseListView::onRenameCanceled() {
	cout << "onRenameCanceled" << endl;
	renameCanceled = true;
}

void aram::gui::BaseListView::onRenameEnded() {
	if (!renameCanceled) {
		Gtk::TreeModel::iterator itr = get_selection()->get_selected();
		if (itr) {
			onRenamed(*itr);
		}
	} else {
		renameCanceled = false;
	}
}

void aram::gui::ProjectListView::onSelected(const Gtk::TreeModel::Row& row) {
	Application app;
	app.load();
	Glib::ustring colId = row[columns.id];
	
	cout << row[columns.name] << " selected" << endl;

	if (app.project()->id() != colId.raw()) {
		app.selectProject(colId.raw());
		cout << "new selection" << endl;
		EventBroadcasterSingleton::getInstance().projectChanged();
	} else {
		cout << "same selected" << endl;
	}
}

void aram::gui::ProjectListView::onRenamed(const Gtk::TreeModel::Row& row) {
	Glib::ustring colId = row[columns.id];
	Glib::ustring newName = row[columns.name];

	shared_ptr<Project> proj = Project::retrieveById(colId.raw()); //retrieveCurrent should also do
	proj->rename(newName.raw());
}

void aram::gui::ProjectListView::refresh() {
	listStore->clear();

	Application app;
	app.load();

	for (Project p : Project::findAll()) {
		Gtk::TreeModel::Row row = *listStore->append();
		row[columns.id] = p.id();
		row[columns.name] = p.name();
		row[columns.samplingRate] = p.sampleRate();
		row[columns.length] = p.length();

		if (p == *app.project()) {
			get_selection()->select(row);
		}
	}
}

aram::gui::AudioclipListView::AudioclipListView() :
listStore(Gtk::ListStore::create(columns)) {

	set_model(listStore);

	append_column_editable("Name", columns.name);
	append_column("SampleRate", columns.samplingRate);
	append_column("Length", columns.length);

	refresh();

	EventBroadcasterSingleton::getInstance().projectChangedSignal.connect(
					sigc::mem_fun(this, &AudioclipListView::onReload));

	get_selection()->signal_changed().connect(
					mem_fun(this, &AudioclipListView::onSelectedChanged));

	get_column_cell_renderer(0)->signal_editing_started().connect(
					mem_fun(this, &AudioclipListView::onRenameStarted));

	get_column_cell_renderer(0)->signal_editing_canceled().connect(
					mem_fun(this, &AudioclipListView::onRenameCanceled));
}

void aram::gui::AudioclipListView::onSelected(const Gtk::TreeModel::Row& row) {
	Application app;
	app.load();

	Glib::ustring colId = row[columns.id];
	shared_ptr<Audioclip> selected = app.project()->findAudioclip(colId.raw());

	if (selected != nullptr) {
		app.project()->arm(selected);
	}
}

void aram::gui::AudioclipListView::onRenamed(const Gtk::TreeModel::Row& row) {
	Glib::ustring colId = row[columns.id];
	Glib::ustring newName = row[columns.name];

	shared_ptr<Audioclip> ac = Audioclip::retrieveById(colId.raw());
	ac->rename(newName.raw());
}

void aram::gui::BaseListView::onReload() {
	refresh();
}

void aram::gui::AudioclipListView::refresh() {
	listStore->clear();

	Application app;
	app.load();

	for (shared_ptr<Audioclip> audioclip : app.project()->audioclips()) {
		Gtk::TreeModel::Row row = *(listStore->append());
		row[columns.id] = audioclip->id();
		row[columns.name] = audioclip->name();
		row[columns.samplingRate] = audioclip->sampleRate();
		row[columns.length] = audioclip->length();

		if (*audioclip == app.project()->armed()) {
			get_selection()->select(row);
		}
	}
}
