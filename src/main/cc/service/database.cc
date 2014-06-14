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

#include "database.h"
#include "system.h"
#include "../model/project.h"
#include "../model/project-odb.hxx"
#include "../model/audioclip.h"
#include "../model/audioclip-odb.hxx"
#include <odb/connection.hxx>
#include <odb/transaction.hxx>
#include <odb/schema-catalog.hxx>
#include <exception>
#include <iostream>
#include <algorithm>

using namespace warsaw::model;

warsaw::service::Database::Database() :
db_(new database(System::getHomePath() + "/.warsaw/database",
SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE)) {
	createSchemaNoDrop();
	initApplication();
}

warsaw::service::Database& warsaw::service::Database::getInstance() {
	static Database instance;
	return instance;
}

unique_ptr<database> const& warsaw::service::Database::operator->() {
	return db_;
}

string warsaw::service::Database::generateId() {
	auto randchar = []() -> char {
		const char charset[] =
						"0123456789"
						"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						"abcdefghijklmnopqrstuvwxyz";
		const size_t max_index = (sizeof (charset) - 1);
		return charset[ rand() % max_index ];
	};
	string str(16, 0);
	generate_n(str.begin(), 16, randchar);
	return str;
}

void warsaw::service::Database::createSchemaNoDrop() {
	connection_ptr c(db_->connection());
	c->execute("PRAGMA foreign_keys=OFF");

	try {
		transaction t(c->begin());
		odb::schema_catalog::create_schema(*db_, "", false);
		t.commit();
	} catch (exception& e) {
		cout << "\"" << e.what() << "\". - Looks like the schema exists. Continuing.." << endl;
	}

	c->execute("PRAGMA foreign_keys=ON");
}

void warsaw::service::Database::initApplication() {
	transaction t(db_->begin());
	auto r(db_->query<ApplicationStats>());
	const ApplicationStats & stats(*r.begin());
	t.commit();

	switch (stats.count) {
		case 0:
			createApplication();
			break;
		case 1:
			cout << "Araposer application exists" << endl;
			break;
		default:
			throw runtime_error("Too many applications in the database!");
	}
}

/* We want one Application with one Project with one AudioClip
 */
void warsaw::service::Database::createApplication() {
	cout << "Create Araposer application" << endl;

	try {
		transaction t(db_->begin());

		shared_ptr<Audioclip> ac(new Audioclip("<new audioclip>"));
		db_->persist(ac);

		shared_ptr<Project> p(new Project("<new project>", ac));
		db_->persist(p);

		Application a("aracomposer.application", p);
		
		cout << "Trying to persist application" << endl;
		db_->persist(a);

		t.commit();

		cout << "Application persisted" << endl;
	} catch (const odb::exception& e) {
		throw runtime_error(e.what());
	}
}
