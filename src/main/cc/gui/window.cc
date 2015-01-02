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

#include "window.h"
#include "menudialog.h"
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

		set_default_size(1200, 750);
		set_title("Audio Recorder And Music");
		set_position(Gtk::WindowPosition::WIN_POS_CENTER);

		topContainer.pack_start(commandContainer);
		topContainer.pack_start(bodyContainer);
		add(topContainer);

		show_all_children();

	} catch (const Glib::Error& e) {
		throw std::runtime_error(e.what());
	}
}

/*
 * Command container
 */

aram::gui::CommandContainer::CommandContainer() {

	Gtk::Image* menuImage = Gtk::manage(new Gtk::Image(Gtk::Stock::DIALOG_QUESTION,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	menuImage->show();
	menuButton.set_image(*menuImage);


	Gtk::Image* playImage = Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_PLAY,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	playImage->show();
	playButton.set_image(*playImage);


	Gtk::Image* recordImage = Gtk::manage(new Gtk::Image(Gtk::Stock::MEDIA_RECORD,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	recordImage->show();
	recordButton.set_image(*recordImage);


	Gtk::Image* markImage = Gtk::manage(new Gtk::Image(Gtk::Stock::SELECT_ALL,
					Gtk::BuiltinIconSize::ICON_SIZE_BUTTON));
	markImage->show();
	markButton.set_image(*markImage);

	set_size_request(200, -1);

	menuButton.signal_clicked().connect(
					sigc::mem_fun(this, &CommandContainer::onMenuButtonClicked));

	playButton.signal_clicked().connect(
					sigc::mem_fun(this, &CommandContainer::onPlayButtonClicked));

	recordButton.signal_clicked().connect(
					sigc::mem_fun(this, &CommandContainer::onRecordButtonClicked));

	markButton.signal_pressed().connect(
					sigc::mem_fun(this, &CommandContainer::onMarkButtonPressed));

	markButton.signal_released().connect(
					sigc::mem_fun(this, &CommandContainer::onMarkButtonReleased));

	pack_start(menuButton);
	pack_start(playButton);
	pack_start(recordButton);
	pack_start(markButton);
}

void aram::gui::CommandContainer::onMenuButtonClicked() {
	MenuDialog md;
	md.run();
}

void aram::gui::CommandContainer::onPlayButtonClicked() {
	cout << "Play button clicked" << endl;

	AudioEngine& audioEngine = AudioEngine::getInstance();
	//todo - check actual state of this toggle button
	audioEngine.playback = !audioEngine.playback;
}

void aram::gui::CommandContainer::onRecordButtonClicked() {
	cout << "Record button clicked" << endl;

	AudioEngine& audioEngine = AudioEngine::getInstance();
	//todo - check actual state of this toggle button
	audioEngine.recording = !audioEngine.recording;
}

void aram::gui::CommandContainer::onMarkButtonPressed() {
	cout << "Mark button pressed" << endl;
}

void aram::gui::CommandContainer::onMarkButtonReleased() {
	cout << "Mark button released" << endl;
}

/*
 * Body container
 */
aram::gui::Monitor::Monitor() {
	
}

aram::gui::ProjectView::ProjectView() {
	
}

aram::gui::BodyContainer::BodyContainer() {
	set_size_request(1000, -1);

	pack_start(projectMonitor);
	pack_start(projectView);
	pack_start(audioclipMonitor);
}
