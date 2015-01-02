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
#include <iostream>

#include "audioclip.h"
#include "audioclip-odb.hxx"
#include "../service/database.h"
#include "project.h"
#include "project-odb.hxx"

using namespace aram::service;

aram::model::Audioclip::Audioclip() {
}

aram::model::Audioclip::Audioclip(const string& n) : id_(aram::service::Database::generateId()), 
				name_(n), 
				sampleRate_(1010101) {
}

aram::model::Audioclip::~Audioclip() {
}

bool aram::model::Audioclip::operator==(const Audioclip& ac) const {
	return id() == ac.id();
}

bool aram::model::Audioclip::operator<(const Audioclip& other) const {
	return name_ < other.name_;
}

const string& aram::model::Audioclip::id() const {
	return id_;
}

const string& aram::model::Audioclip::name() const {
	return name_;
}

void aram::model::Audioclip::name(const string& name) {
	name_ = name;
}

unsigned aram::model::Audioclip::length() const {
	return 123;
}

const unsigned& aram::model::Audioclip::sampleRate() const {
	return sampleRate_;
}

void aram::model::Audioclip::createNew() {
	shared_ptr<Project> project = Project::retrieveCurrent();

	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());

		//todo, test with unique_ptr
		shared_ptr<Audioclip> audioclip(new Audioclip("<new audioclip>"));
		db->persist(audioclip);
		
		project->addAudioclip(audioclip);
		db->update(project);

		t.commit();
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}

shared_ptr<aram::model::Audioclip> aram::model::Audioclip::retrieveById(const string& audioclipId) {
	try {
		Database& db = Database::getInstance();
		transaction t(db->begin());
		typedef odb::query<Audioclip> AudioclipQ;
		auto r(db->query<Audioclip>(AudioclipQ::id == audioclipId));
		list<Audioclip> audioclips;
		for (odb::result<Audioclip>::iterator i(r.begin()); i != r.end(); i++) {
			audioclips.push_back(*i);
		}
		t.commit();
		if (audioclips.size() != 1) {
			throw runtime_error("Couldn't find one and only one Audioclip for id=" + audioclipId);
		}
		shared_ptr<Audioclip> ac(new Audioclip(audioclips.front()));
		return ac;
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}

void aram::model::Audioclip::rename(const string& newName) {
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


bool aram::model::Audioclip::Less::operator() (const shared_ptr<Audioclip>& a, const shared_ptr<Audioclip>& b) {
	return (*a) < (*b);
}