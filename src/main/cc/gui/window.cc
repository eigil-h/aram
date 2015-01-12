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

using namespace aram::service;

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
		set_title("Audio Recorder And Music");
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
	pack_start(trackBox);
}

aram::gui::ReceivingTrackBox::ReceivingTrackBox() :
combo(true) {

	combo.set_model((comboModelRef = Gtk::ListStore::create(comboModel)));

	{
		Gtk::TreeModel::Row row = *(comboModelRef->append());
		row[comboModel.trackId] = "track#1";
		row[comboModel.trackName] = "Groovy bass";
	}
	{
		Gtk::TreeModel::Row row = *(comboModelRef->append());
		row[comboModel.trackId] = "track#2";
		row[comboModel.trackName] = "Groovy guitar";
	}

	combo.set_entry_text_column(comboModel.trackName);
	combo.set_active(0);


	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		entry->add_events(Gdk::FOCUS_CHANGE_MASK);
		entry->signal_changed().connect(
						sigc::mem_fun(this, &ReceivingTrackBox::onSelected));
		entry->signal_activate().connect(
						sigc::mem_fun(this, &ReceivingTrackBox::onActivated));
		focusGainedConnection = entry->signal_focus_in_event().connect(
						sigc::mem_fun(this, &ReceivingTrackBox::onFocusGained));
		focusLostConnection = entry->signal_focus_out_event().connect(
						sigc::mem_fun(this, &ReceivingTrackBox::onFocusLost));
	} else {
		throw runtime_error("receivingTrackCB.get_entry() failed");
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
					sigc::mem_fun(this, &ReceivingTrackBox::onAddTrack));
	removeButton.signal_clicked().connect(
					sigc::mem_fun(this, &ReceivingTrackBox::onRemoveTrack));


	buttonBox.pack_start(addButton);
	buttonBox.pack_start(removeButton);

	pack_start(combo);
	pack_start(buttonBox);
}

aram::gui::ReceivingTrackBox::~ReceivingTrackBox() {
	focusLostConnection.disconnect();
}

aram::gui::ReceivingTrackBox::Model::Model() {
	add(trackId);
	add(trackName);
}

void aram::gui::CommandContainer::onPlayButtonClicked() {
	cout << "Play button clicked" << endl;

	AudioEngine& audioEngine = AudioEngine::getInstance();
	audioEngine.playback = playButton.get_active();
}

void aram::gui::CommandContainer::onRecordButtonClicked() {
	cout << "Record button clicked" << endl;

	AudioEngine& audioEngine = AudioEngine::getInstance();

	if (recordButton.get_active()) {
		audioEngine.armChannel("the_only_one");
	} else {
		audioEngine.disarmChannel();
	}
}

void aram::gui::ReceivingTrackBox::onSelected() {
	cout << "onSelected() " << combo.get_active_row_number() << endl;
	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		cout << "  " << entry->get_text() << endl;

		Gtk::TreeModel::iterator itr = combo.get_active();
		if (itr) {
			Gtk::TreeModel::Row row = *itr;
			cout << "  " << row[comboModel.trackName] << endl;
		} else {
			//Happens when editing
			cout << "  No active" << endl;
		}
	}
}

void aram::gui::ReceivingTrackBox::onActivated() {
	cout << "onEntryActivate() " << combo.get_active_row_number() << endl;
	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		cout << "  " << entry->get_text() << endl;

		Gtk::TreeModel::iterator itr = combo.get_active();
		if (itr) {
			Gtk::TreeModel::Row row = *itr;
			cout << "  " << row[comboModel.trackName] << endl;
		} else {
			//Happens when editing
			cout << "  No active" << endl;
		}
	}
}

bool aram::gui::ReceivingTrackBox::onFocusGained(GdkEventFocus* event) {
	cout << "onFocusGained() " << combo.get_active_row_number() << endl;
	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		cout << "  " << entry->get_text() << endl;

		Gtk::TreeModel::iterator itr = combo.get_active();
		if (itr) {
			Gtk::TreeModel::Row row = *itr;
			cout << "  " << row[comboModel.trackName] << endl;
		} else {
			//Happens when editing
			cout << "  No active" << endl;
		}
	}

	return true;
}

bool aram::gui::ReceivingTrackBox::onFocusLost(GdkEventFocus* event) {
	cout << "onFocusLost() " << combo.get_active_row_number() << endl;
	Gtk::Entry* entry = combo.get_entry();
	if (entry) {
		cout << "  " << entry->get_text() << endl;

		Gtk::TreeModel::iterator itr = combo.get_active();
		if (itr) {
			Gtk::TreeModel::Row row = *itr;
			cout << "  " << row[comboModel.trackName] << endl;
		} else {
			//Happens when editing
			cout << "  No active" << endl;
		}
	}

	return true;
}

void aram::gui::ReceivingTrackBox::onAddTrack() {
	Gtk::TreeModel::Row row = *(comboModelRef->prepend());
	row[comboModel.trackId] = "track#?";
	row[comboModel.trackName] = "<new track>";
	combo.set_active(0);
}

void aram::gui::ReceivingTrackBox::onRemoveTrack() {
}
