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

#include "window.h"
#include <iostream>
#include "../service/audioengine.h"
#include "../model/project.h"
#include "../easylogging++.h"

using namespace aram::service;
using namespace aram::model;

/*
 * Top window
 */
aram::gui::Window::Window() {
	Glib::RefPtr<Gtk::CssProvider> cssProvider = Gtk::CssProvider::create();
	Glib::RefPtr<Gtk::StyleContext> refStyleContext = get_style_context();
	Glib::RefPtr< Gdk::Screen > defaultScreen = Gdk::Screen::get_default();
	refStyleContext->add_provider_for_screen(defaultScreen, cssProvider,
					GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	try {
		cssProvider->load_from_path("style.css");

		set_title("Audio Recorder And Music");
		set_position(Gtk::WindowPosition::WIN_POS_CENTER);

		menuSection1.pack_start(projectMenu);
		menuSections.pack_start(menuSection1);
		menuSection2.pack_start(audioclipMenu);
		menuSection2.pack_start(applicationMenu);
		menuSections.pack_start(menuSection2);
		menuFrame.add(menuSections);
		frames.pack_start(menuFrame);
		bodySections.pack_start(audioclipView);
		bodySections.pack_start(projectView);
		bodyFrame.add(bodySections);
		frames.pack_start(bodyFrame);
		add(frames);
		show_all_children();

	} catch (const Glib::Error& e) {
		throw std::runtime_error(e.what());
	}
}

static Gtk::Image* getStockImage(const Gtk::StockID& stockId) {
	Gtk::Image* img = Gtk::manage(new Gtk::Image(stockId,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	img->show();
	return img;
}

aram::gui::ProjectMenu::ProjectMenu() {
	create.set_image(*getStockImage(Gtk::Stock::NEW));
	open.set_image(*getStockImage(Gtk::Stock::OPEN));
	edit.set_image(*getStockImage(Gtk::Stock::EDIT));

	shared_ptr<Project> project = Project::retrieveCurrent();
	stats.set_text(project->name() + " @ " + to_string(project->sampleRate()) + "Hz");

	create.signal_clicked().connect(sigc::mem_fun(this, &ProjectMenu::onCreate));
	open.signal_clicked().connect(sigc::mem_fun(this, &ProjectMenu::onOpen));
	edit.signal_clicked().connect(sigc::mem_fun(this, &ProjectMenu::onEdit));
	
	GuiSignal::getInstance().anotherProjectSelected.connect(
					sigc::mem_fun(this, &ProjectMenu::onStatsChanged));

	GuiSignal::getInstance().projectEdited.connect(
					sigc::mem_fun(this, &ProjectMenu::onStatsChanged));

	pack_start(create);
	pack_start(open);
	pack_start(edit);
	pack_start(stats);
}

void aram::gui::ProjectMenu::onCreate() {
	shared_ptr<Project> project = Project::retrieveCurrent();

	EditDialog dialog("Create a new project", project->name());
	int result = dialog.run();
	switch (result) {
		case Gtk::RESPONSE_OK: {
			if (dialog.name() != project->name()) {
				project = Project::createNew();
				project->rename(dialog.name());
				Project::setCurrent(project);
				GuiSignal::getInstance().anotherProjectSelected();
				LOG(INFO) << "New project \"" << dialog.name() << "\" created and selected.";
			} else {
				LOG(DEBUG) << "RESPONSE_OK with same name";
			}
			break;
		}
		case Gtk::RESPONSE_CANCEL:
		case Gtk::RESPONSE_DELETE_EVENT:
			LOG(DEBUG) << "RESPONSE_CANCEL or RESPONSE_DELETE_EVENT";
			break;
		default:
			LOG(WARNING) << result;
	}
}

void aram::gui::ProjectMenu::onOpen() {
	shared_ptr<Project> project = Project::retrieveCurrent();

	OpenProjectDialog dialog;
	int result = dialog.run();
	switch (result) {
		case Gtk::RESPONSE_OK: {
			if(dialog.selectedId() != project->id()) {
				project = Project::retrieveById(dialog.selectedId());
				Project::setCurrent(project);
				GuiSignal::getInstance().anotherProjectSelected();
				LOG(INFO) << "\"" << project->name() << "\" selected.";
			} else {
				LOG(DEBUG) << "RESPONSE_OK with same name";
			}
			break;
		}
		case Gtk::RESPONSE_CANCEL:
		case Gtk::RESPONSE_DELETE_EVENT:
			LOG(DEBUG) << "RESPONSE_CANCEL or RESPONSE_DELETE_EVENT";
			break;
		default:
			LOG(WARNING) << result;
	}
}

void aram::gui::ProjectMenu::onEdit() {
	shared_ptr<Project> project = Project::retrieveCurrent();

	EditDialog dialog("Edit the project", project->name());
	int result = dialog.run();
	switch (result) {
		case Gtk::RESPONSE_OK: {
			if (dialog.name() != project->name()) {
				string oldname = project->name();
				project->rename(dialog.name());
				GuiSignal::getInstance().projectEdited();
				LOG(INFO) << "\"" << oldname << "\" renamed to \"" << dialog.name() << "\"";
			} else {
				LOG(DEBUG) << "RESPONSE_OK with same name";
			}
			break;
		}
		case Gtk::RESPONSE_CANCEL:
		case Gtk::RESPONSE_DELETE_EVENT:
			LOG(DEBUG) << "RESPONSE_CANCEL or RESPONSE_DELETE_EVENT";
			break;
		default:
			LOG(WARNING) << result;
	}
}

void aram::gui::ProjectMenu::onStatsChanged() {
	shared_ptr<Project> project = Project::retrieveCurrent();
	stats.set_text(project->name() + " @ " + to_string(project->sampleRate()) + "Hz");
}


aram::gui::AudioclipMenu::AudioclipMenu() {
	create.set_image(*getStockImage(Gtk::Stock::NEW));
	open.set_image(*getStockImage(Gtk::Stock::OPEN));
	edit.set_image(*getStockImage(Gtk::Stock::EDIT));

	shared_ptr<Audioclip> ac = Audioclip::retrieveCurrent();
	stats.set_text(ac->name()); 

	create.signal_clicked().connect(sigc::mem_fun(this, &AudioclipMenu::onCreate));
	open.signal_clicked().connect(sigc::mem_fun(this, &AudioclipMenu::onOpen));
	edit.signal_clicked().connect(sigc::mem_fun(this, &AudioclipMenu::onEdit));

	GuiSignal::getInstance().anotherAudioclipSelected.connect(
					sigc::mem_fun(this, &AudioclipMenu::onStatsChanged));

	GuiSignal::getInstance().audioclipEdited.connect(
					sigc::mem_fun(this, &AudioclipMenu::onStatsChanged));

	pack_start(stats);
	pack_start(create);
	pack_start(open);
	pack_start(edit);
}

void aram::gui::AudioclipMenu::onCreate() {
	shared_ptr<Audioclip> audioclip = Audioclip::retrieveCurrent();

	EditDialog dialog("Create a new audioclip", audioclip->name());
	int result = dialog.run();
	switch (result) {
		case Gtk::RESPONSE_OK:
		{
			if (dialog.name() != audioclip->name()) {
				audioclip = Audioclip::createNew();
				audioclip->rename(dialog.name());
				Audioclip::setCurrent(audioclip);
				GuiSignal::getInstance().anotherAudioclipSelected();
				LOG(INFO) << "New audioclip \"" << dialog.name() << "\" created and selected.";
			} else {
				LOG(DEBUG) << "RESPONSE_OK with same name";
			}
			break;
		}
		case Gtk::RESPONSE_CANCEL:
		case Gtk::RESPONSE_DELETE_EVENT:
			LOG(DEBUG) << "RESPONSE_CANCEL or RESPONSE_DELETE_EVENT";
			break;
		default:
			LOG(WARNING) << result;
	}
}

void aram::gui::AudioclipMenu::onOpen() {
	shared_ptr<Audioclip> audioclip = Audioclip::retrieveCurrent();

	OpenAudioclipDialog dialog;
	int result = dialog.run();
	switch (result) {
		case Gtk::RESPONSE_OK:
		{
			if (dialog.selectedId() != audioclip->id()) {
				audioclip = Audioclip::retrieveById(dialog.selectedId());
				Audioclip::setCurrent(audioclip);
				GuiSignal::getInstance().anotherAudioclipSelected();
				LOG(INFO) << "\"" << audioclip->name() << "\" selected.";
			} else {
				LOG(DEBUG) << "RESPONSE_OK with same name";
			}
			break;
		}
		case Gtk::RESPONSE_CANCEL:
		case Gtk::RESPONSE_DELETE_EVENT:
			LOG(DEBUG) << "RESPONSE_CANCEL or RESPONSE_DELETE_EVENT";
			break;
		default:
			LOG(WARNING) << result;
	}
}

void aram::gui::AudioclipMenu::onEdit() {
	shared_ptr<Audioclip> audioclip = Audioclip::retrieveCurrent();

	EditDialog dialog("Edit the audioclip", audioclip->name());
	int result = dialog.run();
	switch (result) {
		case Gtk::RESPONSE_OK:
		{
			if (dialog.name() != audioclip->name()) {
				string oldname = audioclip->name();
				audioclip->rename(dialog.name());
				GuiSignal::getInstance().audioclipEdited();
				LOG(INFO) << "\"" << oldname << "\" renamed to \"" << dialog.name() << "\"";
			} else {
				LOG(DEBUG) << "RESPONSE_OK with same name";
			}
			break;
		}
		case Gtk::RESPONSE_CANCEL:
		case Gtk::RESPONSE_DELETE_EVENT:
			LOG(DEBUG) << "RESPONSE_CANCEL or RESPONSE_DELETE_EVENT";
			break;
		default:
			LOG(WARNING) << result;
	}
}

void aram::gui::AudioclipMenu::onStatsChanged() {
	shared_ptr<Audioclip> ac = Audioclip::retrieveCurrent();
	stats.set_text(ac->name());
}

aram::gui::ApplicationMenu::ApplicationMenu() {
	undo.set_image(*getStockImage(Gtk::Stock::UNDO));
	prefs.set_image(*getStockImage(Gtk::Stock::PREFERENCES));
	help.set_image(*getStockImage(Gtk::Stock::HELP));

	pack_start(undo);
	pack_start(prefs);
	pack_start(help);
}

aram::gui::Navigator::Navigator() {
	start.set_image(*getStockImage(Gtk::Stock::MEDIA_PREVIOUS));
	rewind.set_image(*getStockImage(Gtk::Stock::MEDIA_REWIND));
	backpedal.set_image(*getStockImage(Gtk::Stock::GO_BACK));
	forward.set_image(*getStockImage(Gtk::Stock::GO_FORWARD));
	fastforward.set_image(*getStockImage(Gtk::Stock::MEDIA_FORWARD));
	end.set_image(*getStockImage(Gtk::Stock::MEDIA_NEXT));

	pack_start(start);
	pack_start(rewind);
	pack_start(backpedal);
	pack_start(forward);
	pack_start(fastforward);
	pack_start(end);
}

aram::gui::AudioclipView::AudioclipView() {
	record.set_image(*getStockImage(Gtk::Stock::MEDIA_RECORD));
	mark.set_label("Mark");
	unmark.set_label("Unmark");
	play.set_image(*getStockImage(Gtk::Stock::MEDIA_PLAY));
	counter.set_label("0 / 0");
	trimFront.set_label("Trim front");
	trimBack.set_label("Trim back");
	assign.set_label("Assign");
	unassign.set_label("Unassign");

	attach(record, 0, 0, 2, 2);
	attach(mark, 2, 0, 1, 2);
	attach(unmark, 3, 0, 1, 2);
	attach(play, 0, 2, 2, 2);
	attach(counter, 2, 2, 2, 2);
	attach(navigator, 0, 4, 4, 2);
	attach(trimFront, 0, 6, 1, 1);
	attach(trimBack, 0, 7, 1, 1);
	attach(assign, 2, 6, 2, 2);
	attach(unassign, 2, 8, 2, 1);
}

aram::gui::ProjectView::ProjectView() {
	counter.set_label("0 / 0");
	play.set_image(*getStockImage(Gtk::Stock::MEDIA_PLAY));
	loop.set_label("Loop");
	addChannel.set_label("Add channel");

	attach(counter, 0, 0, 2, 2);
	attach(navigator, 2, 0, 4, 2);
	attach(play, 0, 2, 2, 4);
	attach(channelView, 2, 2, 4, 10);
	attach(loop, 0, 6, 2, 1);
	attach(addChannel, 0, 11, 2, 1);
}

aram::gui::ModelDialog::ModelDialog(const string& title) : Gtk::Dialog(title, true) {
	add_button(Gtk::Stock::OK, Gtk::RESPONSE_OK);
	add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
}

void aram::gui::ModelDialog::onActivateResponseOK() {
	response(Gtk::RESPONSE_OK);
}

aram::gui::OpenDialog::OpenDialog(const string& title) : ModelDialog(title) {
	list.set_model((listModelRef = Gtk::ListStore::create(listModel)));
}

aram::gui::OpenProjectDialog::OpenProjectDialog() : OpenDialog("Open a project") {
	set<Project> projects = Project::findAll();
	for (Project p : projects) {
		Gtk::TreeModel::Row row = *(listModelRef->append());
		row[listModel.id] = p.id();
		row[listModel.name] = p.name();
	}
	list.append_column("Name", listModel.name);
	get_content_area()->pack_start(list);
	show_all();
}

aram::gui::OpenAudioclipDialog::OpenAudioclipDialog() : OpenDialog("Open an audioclip") {
	OrderedAudioclipSet audioclips = Audioclip::findAll();
	for (shared_ptr<Audioclip> ac : audioclips) {
		Gtk::TreeModel::Row row = *(listModelRef->append());
		row[listModel.id] = ac->id();
		row[listModel.name] = ac->name();
	}
	list.append_column("Name", listModel.name);
	get_content_area()->pack_start(list);
	show_all();
}

aram::gui::OpenDialog::Model::Model() {
	add(id);
	add(name);
}

string aram::gui::OpenDialog::selectedId() {
	Glib::RefPtr<Gtk::TreeView::Selection> selection = list.get_selection();
	if (selection) {
		Gtk::TreeModel::iterator itr = selection->get_selected();
		if(itr) {
			Glib::ustring selectedId = (*itr)[listModel.id];
			return selectedId;
		}
	}
	return "";
}

aram::gui::EditDialog::EditDialog(const string& title, const string& name) :
			ModelDialog(title), renameLabel("New name") {
	renameEntry.set_text(name);
	renameEntry.select_region(0, name.length());
	renameEntry.signal_activate().connect(sigc::mem_fun(this, &EditDialog::onActivateResponseOK));

	renameBox.pack_start(renameLabel);
	renameBox.pack_start(renameEntry);
	get_content_area()->pack_start(renameBox);
	show_all();
}

string aram::gui::EditDialog::name() const {
	return renameEntry.get_text();
}

aram::gui::GuiSignal& aram::gui::GuiSignal::getInstance() {
	static GuiSignal instance;
	return instance;
}
