/*
 * File:   window.h
 * Author: eigil
 *
 * Created on February 5, 2014, 1:14 PM
 */
#include <gtkmm.h>
#include "../application.h"

#ifndef WARZAW_WINDOW_H
#define WARZAW_WINDOW_H

namespace warsaw {
	namespace gui {

		class ProjectContainer : public Gtk::HBox {
			Gtk::Button playButton;
			Gtk::Label tmp;
		public:
			ProjectContainer();
		};

		class AudioclipContainer : public Gtk::HBox {
			Gtk::VBox buttonContainer;
			Gtk::Button recordButton;
			Gtk::Button markButton;
			Gtk::Label tmp;
		public:
			AudioclipContainer();
		};

		class Window : public Gtk::Window {
			Gtk::VBox mainContainer;
			ProjectContainer projectContainer;
			AudioclipContainer audioclipContainer;

		public:

			Window();
		};
	}

	class GtkmmApplication : public Application {
		Glib::RefPtr<Gtk::Application> app;

	public:
		GtkmmApplication(int argc, char** argv);

		virtual void run() throw (std::exception);
	};
}

#endif
