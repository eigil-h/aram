/*
	Warsaw, the audio recorder and music composer
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

#include "project.h"
#include "project-odb.hxx"
#include "../service/database.h"
#include <iostream>

using namespace warsaw::service;

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Project
 */

warsaw::model::Project::Project() {
}

warsaw::model::Project::Project(const string& n, const shared_ptr<AudioClip>& ac) : 
		id_(warsaw::service::Database::generateId()), name_(n), frames_(0), armed_(ac) {
	audioClips_.insert(ac);
}

warsaw::model::Project::~Project() {
}

const string& warsaw::model::Project::name() const {
	return name_;
}

const unsigned& warsaw::model::Project::frames() const {
	return frames_;
}

void warsaw::model::Project::audioEngineProcessedFrames(unsigned nFrames) {
	frames_ += nFrames;
}

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Application
 */
warsaw::model::Application::Application() {
}

warsaw::model::Application::Application(const string& name, const shared_ptr<Project>& project) :
name_(name), project_(project) {
	cout << name << " created" << endl;
}

const string& warsaw::model::Application::name() const {
	return name_;
}

const shared_ptr<warsaw::model::Project>& warsaw::model::Application::project() const {
	return project_;
}

void warsaw::model::Application::load() {
	Database& db = Database::getInstance();

	transaction t(db->begin());

	try {
		db->load("aracomposer.application", *this);

		t.commit();
	} catch (odb::object_not_persistent e) {
		cout << e.what() << endl;
	}
}