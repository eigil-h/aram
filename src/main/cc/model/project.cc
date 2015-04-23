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

#include "project.h"
#include "project-odb.hxx"
#include "../service/database.h"
#include "src/main/cc/service/system.h"
#include <iostream>
#include <algorithm>
#include "../easylogging++.h"


using namespace aram::service;

/*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx*xXx
 * Project
 */

aram::model::Project::Project() {
}

aram::model::Project::Project(const string& n, const shared_ptr<Channel>& ch) :
id_(aram::service::Database::generateId()), name_(n), frames_(0), sampleRate_(22049) {
	channels_.push_front(ch);
	audioclips_.insert(ch->firstAudioclip());
}

aram::model::Project::~Project() {
}

bool aram::model::Project::operator==(const Project& other) const {
	return id() == other.id();
}

bool aram::model::Project::operator<(const Project& other) const {
	return name() < other.name();
}

const string& aram::model::Project::id() const {
	return id_;
}

const string& aram::model::Project::name() const {
	return name_;
}

const set<shared_ptr<aram::model::Audioclip>, aram::model::Audioclip::Less>& 
				aram::model::Project::audioclips() const {
	return audioclips_;
}

const list<shared_ptr<aram::model::Channel>>& aram::model::Project::channels() const {
	return channels_;
}

shared_ptr<aram::model::Audioclip> aram::model::Project::findAudioclip(const string& id) const {
	for(shared_ptr<Audioclip> ac : audioclips_) {
		if(ac->id() == id) {
			return ac;
		}
	}
	return nullptr;
}

const unsigned& aram::model::Project::frames() const {
	return frames_;
}

aram::service::PlaybackPos aram::model::Project::length() const {
	return (*max_element(channels().begin(), channels().end(), 
					[](const shared_ptr<Channel>& a, const shared_ptr<Channel>& b) {
		if(b->length() > a->length()) {
			return b;
		}
		return a;
	}))->length();
}

const unsigned& aram::model::Project::sampleRate() const {
	return sampleRate_;
}

void aram::model::Project::addAudioclip(shared_ptr<Audioclip> ac) {
	audioclips_.insert(ac);
}

void aram::model::Project::audioEngineProcessedFrames(unsigned nFrames) {
	frames_ += nFrames;
}

void aram::model::Project::rename(const string& newName) {
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

set<aram::model::Project> aram::model::Project::findAll() {
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

shared_ptr<aram::model::Project> aram::model::Project::createNew() {
	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());

		//todo, test with unique_ptr
		shared_ptr<Audioclip> ac(new Audioclip("<new audioclip>"));
		db->persist(ac);

		shared_ptr<Channel> ch(new Channel("<new channel>", ac));
		db->persist(ch);

		shared_ptr<Project> p(new Project("<new project>", ch));
		db->persist(p);

		t.commit();
		return p;
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}

shared_ptr<aram::model::Project> aram::model::Project::retrieveCurrent() {
	Application app;
	app.load();
	return app.project();
}

void aram::model::Project::setCurrent(shared_ptr<Project> project) {
	Application app;
	app.load();
	app.selectProject(project->id());
	
}

shared_ptr<aram::model::Project> aram::model::Project::retrieveById(const string& projectId) {
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
aram::model::Application::Application() {
	System::mkdir(System::getHomePath() + "/.aram");
}

aram::model::Application::Application(const string& name, const shared_ptr<Project>& project) :
name_(name), project_(project) {
	LOG(INFO) << name << " created";
}

const string& aram::model::Application::name() const {
	return name_;
}

const shared_ptr<aram::model::Project>& aram::model::Application::project() const {
	return project_;
}

void aram::model::Application::load() {
	Database& db = Database::getInstance();

	transaction t(db->begin());

	try {
		db->load("aram.application", *this);

		t.commit();
	} catch (odb::object_not_persistent e) {
		LOG(ERROR) << e.what();
	}
}

void aram::model::Application::selectProject(const string& id) {
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