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

		set_default_size(800, 200);
		shared_ptr<Project> project = Project::retrieveCurrent();
		set_title("Audio Recorder And Music : " + project->name() + " @ " + to_string(project->sampleRate()) + "Hz");
		set_position(Gtk::WindowPosition::WIN_POS_CENTER);

		add(commandContainer);

		show_all_children();

	} catch (const Glib::Error& e) {
		throw std::runtime_error(e.what());
	}
}

/*
 * Command container
 */
aram::gui::CommandContainer::CommandContainer() {

	Gtk::Image* playImage = Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_PLAY,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	playImage->show();
	playButton.set_image(*playImage);

	Gtk::Image* recordImage = Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_RECORD,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	recordImage->show();
	recordButton.set_image(*recordImage);


	playButton.signal_clicked().connect(
					sigc::mem_fun(this, &CommandContainer::onPlayButtonClicked));

	recordButton.signal_clicked().connect(
					sigc::mem_fun(this, &CommandContainer::onRecordButtonClicked));

	pack_start(playButton);
	pack_start(recordButton);
	pack_start(channelBox);
}

aram::gui::ReceivingChannelBox::ReceivingChannelBox() :
combo(true) {

	combo.set_model((comboModelRef = Gtk::ListStore::create(comboModel)));

	list<shared_ptr<Channel>> channels = Project::retrieveCurrent()->channels();
	for (shared_ptr<Channel> ch : channels) {
		Gtk::TreeModel::Row row = *(comboModelRef->append());
		row[comboModel.channelId] = ch->id();
		row[comboModel.channelName] = ch->name();
	}

	combo.set_entry_text_column(comboModel.channelName);
	combo.set_active(0);

	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		entry->add_events(Gdk::FOCUS_CHANGE_MASK);
		entry->signal_changed().connect(
						sigc::mem_fun(this, &ReceivingChannelBox::onSelected));
		entry->signal_activate().connect(
						sigc::mem_fun(this, &ReceivingChannelBox::onActivated));
		focusGainedConnection = entry->signal_focus_in_event().connect(
						sigc::mem_fun(this, &ReceivingChannelBox::onFocusGained));
		focusLostConnection = entry->signal_focus_out_event().connect(
						sigc::mem_fun(this, &ReceivingChannelBox::onFocusLost));
	} else {
		throw runtime_error("receivingChannelCB.get_entry() failed");
	}

	Gtk::Image* addImage = Gtk::manage(new Gtk::Image(Gtk::Stock::ADD,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	addImage->show();
	addButton.set_image(*addImage);

	Gtk::Image* removeImage = Gtk::manage(new Gtk::Image(Gtk::Stock::REMOVE,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	removeImage->show();
	removeButton.set_image(*removeImage);

	addButton.signal_clicked().connect(
					sigc::mem_fun(this, &ReceivingChannelBox::onAddChannel));
	removeButton.signal_clicked().connect(
					sigc::mem_fun(this, &ReceivingChannelBox::onRemoveChannel));


	buttonBox.pack_start(addButton);
	buttonBox.pack_start(removeButton);

	pack_start(combo);
	pack_start(buttonBox);
}

aram::gui::ReceivingChannelBox::~ReceivingChannelBox() {
	focusLostConnection.disconnect();
}

aram::gui::ReceivingChannelBox::Model::Model() {
	add(channelId);
	add(channelName);
}

void aram::gui::CommandContainer::onPlayButtonClicked() {
	LOG(INFO) << "Play button clicked";

	AudioEngine& audioEngine = AudioEngine::getInstance();
	audioEngine.playback = playButton.get_active();
}

void aram::gui::CommandContainer::onRecordButtonClicked() {
	LOG(INFO) << "Record button clicked";

	AudioEngine& audioEngine = AudioEngine::getInstance();

	if (recordButton.get_active()) {
		audioEngine.armChannel("the_only_one");
	} else {
		audioEngine.disarmChannel();
	}
}

void aram::gui::ReceivingChannelBox::onSelected() {
	LOG(INFO) << "onSelected() " << combo.get_active_row_number();
	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		LOG(INFO) << "  " << entry->get_text();

		Gtk::TreeModel::iterator itr = combo.get_active();
		if (itr) {
			Gtk::TreeModel::Row row = *itr;
			activeRow = row;
			LOG(INFO) << "  " << row[comboModel.channelName];
		} else {
			//Happens when editing
			LOG(INFO) << "  No active row";
		}
	}
}

void aram::gui::ReceivingChannelBox::onActivated() {
	LOG(INFO) << "onEntryActivate() " << combo.get_active_row_number();
	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		LOG(INFO) << "  " << entry->get_text();

		Gtk::TreeModel::iterator itr = combo.get_active();
		if (itr) {
			Gtk::TreeModel::Row row = *itr;
			activeRow = row;
			LOG(INFO) << "  " << row[comboModel.channelName];
		} else {
			//Happens when editing
			LOG(INFO) << "  No active row";
		}
	}
}

bool aram::gui::ReceivingChannelBox::onFocusGained(GdkEventFocus* event) {
	LOG(INFO) << "onFocusGained() " << combo.get_active_row_number();
	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		LOG(INFO) << "  " << entry->get_text();

		Gtk::TreeModel::iterator itr = combo.get_active();
		if (itr) {
			Gtk::TreeModel::Row row = *itr;
			activeRow = row;
			LOG(INFO) << "  " << row[comboModel.channelName];
		} else {
			//Happens when editing
			LOG(INFO) << "  No active row";
		}
	}

	return true;
}

bool aram::gui::ReceivingChannelBox::onFocusLost(GdkEventFocus* event) {
	LOG(INFO) << "onFocusLost() " << combo.get_active_row_number();
	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		LOG(INFO) << "  " << entry->get_text();

		Gtk::TreeModel::iterator itr = combo.get_active();
		if (itr) {
			Gtk::TreeModel::Row row = *itr;
			activeRow = row;
			LOG(INFO) << "  " << row[comboModel.channelName];
		} else if(activeRow) {
			Gtk::TreeModel::Row row = activeRow;
			row[comboModel.channelName] = entry->get_text();
		} else {
			LOG(INFO) << "  No active row";
		}
	}

	return true;
}

void aram::gui::ReceivingChannelBox::onAddChannel() {
	Gtk::TreeModel::Row row = *(comboModelRef->prepend());
	row[comboModel.channelId] = "channel#?";
	row[comboModel.channelName] = "<new channel>";
	combo.set_active(0);
}

void aram::gui::ReceivingChannelBox::onRemoveChannel() {
}
