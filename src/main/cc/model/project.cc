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
#include <algorithm>


using namespace warsaw::service;

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Project
 */

warsaw::model::Project::Project() {
}

warsaw::model::Project::Project(const string& n, const shared_ptr<Audioclip>& ac) :
id_(warsaw::service::Database::generateId()), name_(n), frames_(0), armed_(ac), sampleRate_(22049) {
	audioclips_.insert(ac);
}

warsaw::model::Project::~Project() {
}

bool warsaw::model::Project::operator==(const Project& other) const {
	return id() == other.id();
}

bool warsaw::model::Project::operator<(const Project& other) const {
	return name() < other.name();
}

const string& warsaw::model::Project::id() const {
	return id_;
}

const string& warsaw::model::Project::name() const {
	return name_;
}

const set<shared_ptr<warsaw::model::Audioclip>, warsaw::model::Audioclip::Less>& 
				warsaw::model::Project::audioclips() const {
	return audioclips_;
}

const warsaw::model::Audioclip& warsaw::model::Project::armed() const {
	return *armed_;
}

shared_ptr<warsaw::model::Audioclip> warsaw::model::Project::findAudioclip(const string& id) const {
	for(shared_ptr<Audioclip> ac : audioclips_) {
		if(ac->id() == id) {
			return ac;
		}
	}
	return nullptr;
}

const unsigned& warsaw::model::Project::frames() const {
	return frames_;
}

unsigned warsaw::model::Project::length() const {
	return 123;
}

const unsigned& warsaw::model::Project::sampleRate() const {
	return sampleRate_;
}

void warsaw::model::Project::addAudioclip(shared_ptr<Audioclip> ac) {
	audioclips_.insert(ac);
}

void warsaw::model::Project::audioEngineProcessedFrames(unsigned nFrames) {
	frames_ += nFrames;
}

void warsaw::model::Project::arm(const shared_ptr<Audioclip>& ac) {
	armed_ = ac; //optimize? - check if this is a change.

	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());

		db->update(*this);

		t.commit();
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}

void warsaw::model::Project::rename(const string& newName) {
	name_ = newName;

	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());

		db->update(*this);

		t.commit();
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}

set<warsaw::model::Project> warsaw::model::Project::findAll() {
	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());
		odb::result<Project> r(db->query<Project>());
		set<Project> projects;
		for (odb::result<Project>::iterator i(r.begin()); i != r.end(); i++) {
			projects.insert(*i); //TODO sort by last accessed desc
		}
		t.commit();
		return projects;
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}

void warsaw::model::Project::createNew() {
	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());

		//todo, test with unique_ptr
		shared_ptr<Audioclip> ac(new Audioclip("<new audioclip>"));
		db->persist(ac);

		shared_ptr<Project> p(new Project("<new project>", ac));
		db->persist(p);

		t.commit();
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}

shared_ptr<warsaw::model::Project> warsaw::model::Project::retrieveCurrent() {
	Application app;
	app.load();
	return app.project();
}

shared_ptr<warsaw::model::Project> warsaw::model::Project::retrieveById(const string& projectId) {
	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());
		typedef odb::query<Project> ProjectQ;
		odb::result<Project> resultSet(db->query<Project>(ProjectQ::id == projectId));
		list<Project> projects;
		for (odb::result<Project>::iterator i(resultSet.begin()); i != resultSet.end(); i++) {
			projects.push_back(*i);
		}
		t.commit();
		if (projects.size() != 1) {
			throw runtime_error("Couldn't find one and only one Project for id=" + projectId);
		}
		shared_ptr<Project> p(new Project(projects.front()));
		return p;
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
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

void warsaw::model::Application::selectProject(const string& id) {
	project_ = Project::retrieveById(id);

	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());

		db->update(this);

		t.commit();
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}